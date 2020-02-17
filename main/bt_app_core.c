/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_log.h"

#include "bt_app_core.h"
#include "bt_app_gap.h"
#include "bt_app_a2d.h"
#include "bt_app_avrc.h"


static bool bt_app_send_msg(bt_app_msg_t *msg);
static void bt_app_work_dispatched(bt_app_msg_t *msg);
static void bt_app_task_handler(void *arg);

static xQueueHandle s_bt_app_task_queue = NULL;
static xTaskHandle s_bt_app_task_handle = NULL;


/* *************************************************************** */
/* *************************************************************** */
void bt_app_classic_mode_init(void)
{
    esp_err_t err;
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((err = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(BT_APP_CORE_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }
    
    if ((err = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(BT_APP_CORE_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }
    
    if ((err = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(BT_APP_CORE_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }
    
    if ((err = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(BT_APP_CORE_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
//static void bt_av_hdl_stack_evt(uint16_t event, void *p_param)
void bt_av_hdl_stack_evt(uint16_t event, void *p_param)
{
    ESP_LOGD(BT_APP_CORE_TAG, "%s evt %d", __func__, event);
    switch (event) {
        case BT_APP_EVT_STACK_UP: {
            /* set up device name */
            char *dev_name = CONFIG_BLUETOOTH_DEVICE_NAME;
            esp_bt_dev_set_device_name(dev_name);
            
            bt_app_gap_init();           /* initialize GAP */
            bt_app_avrc_init();          /* initialize AVRCP controller/target */
            bt_app_a2d_init();           /* initialize A2DP sink */
            bt_app_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);  /* setup gap scan mode */
            break;
        }
        default:
            ESP_LOGE(BT_APP_CORE_TAG, "%s unhandled evt %d", __func__, event);
            break;
    }
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
bool bt_app_work_dispatch(bt_app_cb_t p_cback, uint16_t event, void *p_params, int param_len, bt_app_copy_cb_t p_copy_cback)
{
    ESP_LOGD(BT_APP_CORE_TAG, "%s event 0x%x, param len %d", __func__, event, param_len);
    
    bt_app_msg_t msg;
    memset(&msg, 0, sizeof(bt_app_msg_t));
    
    msg.sig = BT_APP_SIG_WORK_DISPATCH;
    msg.event = event;
    msg.cb = p_cback;
    
    if (param_len == 0) {
        return bt_app_send_msg(&msg);
    } else if (p_params && param_len > 0) {
        if ((msg.param = malloc(param_len)) != NULL) {
            memcpy(msg.param, p_params, param_len);
            /* check if caller has provided a copy callback to do the deep copy */
            if (p_copy_cback) {
                p_copy_cback(&msg, msg.param, p_params);
            }
            return bt_app_send_msg(&msg);
        }
    }
    return false;
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
void bt_app_task_start_up(void)
{
    s_bt_app_task_queue = xQueueCreate(10, sizeof(bt_app_msg_t));
    xTaskCreate(bt_app_task_handler, "bt_app_task_start_up", 3072, NULL, configMAX_PRIORITIES - 3, &s_bt_app_task_handle);
    return;
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
void bt_app_task_shut_down(void)
{
    if (s_bt_app_task_handle) {
        vTaskDelete(s_bt_app_task_handle);
        s_bt_app_task_handle = NULL;
    }
    if (s_bt_app_task_queue) {
        vQueueDelete(s_bt_app_task_queue);
        s_bt_app_task_queue = NULL;
    }
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
static bool bt_app_send_msg(bt_app_msg_t *msg)
{
    if (msg == NULL) {
        return false;
    }
    
    if (xQueueSend(s_bt_app_task_queue, msg, 10 / portTICK_RATE_MS) != pdTRUE) {
        ESP_LOGE(BT_APP_CORE_TAG, "%s xQueue send failed", __func__);
        return false;
    }
    return true;
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
static void bt_app_work_dispatched(bt_app_msg_t *msg)
{
    if (msg->cb) {
        msg->cb(msg->event, msg->param);
    }
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
static void bt_app_task_handler(void *arg)
{
    bt_app_msg_t msg;
    for (;;) {
        if (pdTRUE == xQueueReceive(s_bt_app_task_queue, &msg, (portTickType)portMAX_DELAY)) {
            ESP_LOGD(BT_APP_CORE_TAG, "%s, sig 0x%x, 0x%x", __func__, msg.sig, msg.event);
            switch (msg.sig) {
                case BT_APP_SIG_WORK_DISPATCH:
                    bt_app_work_dispatched(&msg);
                    break;
                default:
                    ESP_LOGW(BT_APP_CORE_TAG, "%s, unhandled sig: %d", __func__, msg.sig);
                    break;
            } // switch (msg.sig)
            
            if (msg.param) {
                free(msg.param);
            }
        }
    }
}
/* *************************************************************** */
/* *************************************************************** */
