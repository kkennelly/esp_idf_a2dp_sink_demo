// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "freertos/FreeRTOS.h"

#include "bt_app_nvs.h"
#include "bt_app_gap.h"
#include "bt_app_i2s.h"
#include "bt_app_core.h"
#include "bt_app_a2d.h"
#include "bt_app_avrc.h"

#define BT_APP_MAIN_TAG      "BT_APP_MAIN"


void app_main()
{
    bt_app_nvs_init();
    
    /* configure the I2S  interface */
    bt_app_i2s_init();
    
    /* initialize Bluetooth Classic Mode */
    bt_app_classic_mode_init();
    
    /* create application task */
    bt_app_task_start_up();
    
    /* Bluetooth device name, connection mode and profile set up */
    bt_app_work_dispatch(bt_av_hdl_stack_evt, BT_APP_EVT_STACK_UP, NULL, 0, NULL);
    
    /* initialize GAP SSP */
    bt_app_gap_ssp_config();
}
