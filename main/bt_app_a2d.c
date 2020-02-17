
/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"

#include "esp_a2dp_api.h"
#include "esp_log.h"

#include "bt_app_gap.h"
#include "bt_app_i2s.h"
#include "bt_app_core.h"
#include "bt_app_a2d.h"


static void bt_av_hdl_a2d_evt(uint16_t event, void *p_param);  /* a2dp event handler */

static uint32_t s_pkt_cnt = 0;
static esp_a2d_audio_state_t s_audio_state = ESP_A2D_AUDIO_STATE_STOPPED;
static const char *s_a2d_conn_state_str[] = {"Disconnected", "Connecting", "Connected", "Disconnecting"};
static const char *s_a2d_audio_state_str[] = {"Suspended", "Stopped", "Started"};


/* *************************************************************** */
/* *************************************************************** */
void bt_app_a2d_init()
{
    esp_a2d_register_callback(&bt_app_a2d_cb);
    esp_a2d_sink_register_data_callback(bt_app_a2d_data_cb);
    esp_a2d_sink_init();
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
        case ESP_A2D_AUDIO_STATE_EVT:
        case ESP_A2D_AUDIO_CFG_EVT: {
            bt_app_work_dispatch(bt_av_hdl_a2d_evt, event, param, sizeof(esp_a2d_cb_param_t), NULL);
            break;
        }
        default:
            ESP_LOGE(BT_APP_A2D_TAG, "Invalid A2DP event: %d", event);
            break;
    }
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{
    write_ringbuf(data, len);
    if (++s_pkt_cnt % 100 == 0) {
        ESP_LOGI(BT_APP_A2D_TAG, "Audio packet count %u", s_pkt_cnt);
    }
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
static void bt_av_hdl_a2d_evt(uint16_t event, void *p_param)
{
    ESP_LOGD(BT_APP_A2D_TAG, "%s evt %d", __func__, event);
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT: {
            a2d = (esp_a2d_cb_param_t *)(p_param);
            uint8_t *bda = a2d->conn_stat.remote_bda;
            ESP_LOGI(BT_APP_A2D_TAG, "A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
                     s_a2d_conn_state_str[a2d->conn_stat.state], bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                bt_app_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
                bt_i2s_task_shut_down();
            } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED){
                bt_app_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
                bt_i2s_task_start_up();
            }
            break;
        }
        case ESP_A2D_AUDIO_STATE_EVT: {
            a2d = (esp_a2d_cb_param_t *)(p_param);
            ESP_LOGI(BT_APP_A2D_TAG, "A2DP audio state: %s", s_a2d_audio_state_str[a2d->audio_stat.state]);
            s_audio_state = a2d->audio_stat.state;
            if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
                s_pkt_cnt = 0;
            }
            break;
        }
        case ESP_A2D_AUDIO_CFG_EVT: {
            a2d = (esp_a2d_cb_param_t *)(p_param);
            ESP_LOGI(BT_APP_A2D_TAG, "A2DP audio stream configuration, codec type %d", a2d->audio_cfg.mcc.type);
            // for now only SBC stream is supported
            if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
                int sample_rate = 16000;
                char oct0 = a2d->audio_cfg.mcc.cie.sbc[0];
                if (oct0 & (0x01 << 6)) {
                    sample_rate = 32000;
                } else if (oct0 & (0x01 << 5)) {
                    sample_rate = 44100;
                } else if (oct0 & (0x01 << 4)) {
                    sample_rate = 48000;
                }
                bt_i2s_set_clk(0, sample_rate, 16, 2);
                
                ESP_LOGI(BT_APP_A2D_TAG, "Configure audio player %x-%x-%x-%x",
                         a2d->audio_cfg.mcc.cie.sbc[0],
                         a2d->audio_cfg.mcc.cie.sbc[1],
                         a2d->audio_cfg.mcc.cie.sbc[2],
                         a2d->audio_cfg.mcc.cie.sbc[3]);
                ESP_LOGI(BT_APP_A2D_TAG, "Audio player configured, sample rate=%d", sample_rate);
            }
            break;
        }
        default:
            ESP_LOGE(BT_APP_A2D_TAG, "%s unhandled evt %d", __func__, event);
            break;
    }
}
/* *************************************************************** */
/* *************************************************************** */
