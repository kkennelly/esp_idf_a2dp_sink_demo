/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __BT_APP_A2D_H__
#define __BT_APP_A2D_H__

#include <stdint.h>
#include "esp_a2dp_api.h"

#define BT_APP_A2D_TAG      "BT_APP_A2D"


void bt_app_a2d_init();

/**
 * @brief     callback function for A2DP sink
 */
void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

/**
 * @brief     callback function for A2DP sink audio data stream
 */
void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len);


#endif /* __BT_APP_A2D_H__ */
