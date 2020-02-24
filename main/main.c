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

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bt_app_nvs.h"
#include "bt_app_gap.h"
#include "bt_app_i2s.h"
#include "bt_app_core.h"
#include "bt_app_a2d.h"
#include "bt_app_avrc.h"
#include "esp_avrc_api.h"

#include "tftspi.h"
#include "tft.h"
#include "tftfontfiles.c"
#include "touch.h"
#include "esp_spiffs.h"
#include "esp_log.h"

#include "bt_lcd.h"


#define BT_APP_MAIN_TAG      "BT_APP_MAIN"

void app_main()
{
    setupBT();
	
	setupScreen();
	
	//tft_demo();
	
	struct song_info {
		char *title;
		char *artist;
		char *album;
	};
	
	
	
	//TFT_print(meta_rsp.attr_text, 115, 35);
}







