/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __BT_APP_AVRC_H__
#define __BT_APP_AVRC_H__

#include <stdint.h>
#include "esp_avrc_api.h"

#define BT_APP_AVRC_TAG     "BT_APP_AVRC"
#define BT_APP_RC_TG_TAG    "BT_APP_RC_TG"
#define BT_APP_RC_CT_TAG    "BT_APP_RC_CT"


void bt_app_avrc_init();

/**
 * @brief     callback function for AVRCP controller
 */
void bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);

/**
 * @brief     callback function for AVRCP target
 */
void bt_app_rc_tg_cb(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param);

#endif /* __BT_APP_AVRC_H__ */
