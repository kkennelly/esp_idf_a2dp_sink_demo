/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_gap_bt_api.h"
#include "esp_log.h"

#include "bt_app_gap.h"


/* *************************************************************** */
/* *************************************************************** */
void bt_app_gap_init()
{
    esp_bt_gap_register_callback(bt_app_gap_cb);
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
void bt_app_gap_set_scan_mode(esp_bt_connection_mode_t c_mode, esp_bt_discovery_mode_t d_mode)
{
    /* set discoverable and connectable mode, wait to be connected */
    esp_bt_gap_set_scan_mode(c_mode, d_mode);
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
void bt_app_gap_ssp_config()
{
#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif
    
    /*
     * Set default parameters for Legacy Pairing
     * Use fixed pin code
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
    esp_bt_pin_code_t pin_code;
    pin_code[0] = '1';
    pin_code[1] = '2';
    pin_code[2] = '3';
    pin_code[3] = '4';
    esp_bt_gap_set_pin(pin_type, 4, pin_code);
}
/* *************************************************************** */
/* *************************************************************** */


/* *************************************************************** */
/* *************************************************************** */
void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_BT_GAP_AUTH_CMPL_EVT: {
            if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(BT_APP_GAP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
                esp_log_buffer_hex(BT_APP_GAP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
            } else {
                ESP_LOGE(BT_APP_GAP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
            }
            break;
        }
            
#if (CONFIG_BT_SSP_ENABLED == true)
        case ESP_BT_GAP_CFM_REQ_EVT:
            ESP_LOGI(BT_APP_GAP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
            esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
            break;
        case ESP_BT_GAP_KEY_NOTIF_EVT:
            ESP_LOGI(BT_APP_GAP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
            break;
        case ESP_BT_GAP_KEY_REQ_EVT:
            ESP_LOGI(BT_APP_GAP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
            break;
#endif
            
        default: {
            ESP_LOGI(BT_APP_GAP_TAG, "event: %d", event);
            break;
        }
    }
    return;
}
/* *************************************************************** */
/* *************************************************************** */
