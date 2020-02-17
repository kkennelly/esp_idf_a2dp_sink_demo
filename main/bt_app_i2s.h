/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __BT_APP_I2S_H__
#define __BT_APP_I2S_H__

#include <stdint.h>
#include "driver/i2s.h"

#define BT_APP_I2S_TAG      "BT_APP_I2S"


void bt_app_i2s_init();

void bt_i2s_task_start_up(void);

void bt_i2s_task_shut_down(void);

void bt_i2s_set_clk(i2s_port_t i2s_num, uint32_t rate, i2s_bits_per_sample_t bits, i2s_channel_t ch);

size_t write_ringbuf(const uint8_t *data, size_t size);

#endif /* __BT_APP_I2S_H__ */
