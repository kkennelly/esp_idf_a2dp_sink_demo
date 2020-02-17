/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __BT_APP_GAP_H__
#define __BT_APP_GAP_H__

#include "esp_gap_bt_api.h"

#define BT_APP_GAP_TAG      "BT_APP_GAP"


void bt_app_gap_init();

void bt_app_gap_set_scan_mode(esp_bt_connection_mode_t c_mode, esp_bt_discovery_mode_t d_mode);

void bt_app_gap_ssp_config();

void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

#endif /* __BT_APP_GAP_H__ */
