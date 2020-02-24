#include "bt_lcd.h"

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

// ==========================================================
// Define which spi bus to use TFT_VSPI_HOST or TFT_HSPI_HOST
#define SPI_BUS TFT_HSPI_HOST
// ==========================================================

#define DEFAULT_TFT_ORIENTATION PORTRAIT

void bt_app_updateUI(int id, char* text) {

}

void setupBT() {
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

void setupScreen() {
	
	esp_err_t ret;

	// ===================================================
	// ==== Set maximum spi clock for display read    ====
	//      operations, function 'find_rd_speed()'    ====
	//      can be used after display initialization  ====
	tft_max_rdclock = 8000000;
	// ===================================================

    // ====================================================================
    // === Pins MUST be initialized before SPI interface initialization ===
    // ====================================================================
    TFT_PinsInit();

    // ====  CONFIGURE SPI DEVICES(s)  ====================================================================================

    spi_lobo_device_handle_t spi;
	
    spi_lobo_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,				// set SPI MISO pin
        .mosi_io_num=PIN_NUM_MOSI,				// set SPI MOSI pin
        .sclk_io_num=PIN_NUM_CLK,				// set SPI CLK pin
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
		.max_transfer_sz = 6*1024,
    };
    spi_lobo_device_interface_config_t devcfg={
        .clock_speed_hz=8000000,                // Initial clock out at 8 MHz
        .mode=0,                                // SPI mode 0
        .spics_io_num=-1,                       // we will use external CS pin
		.spics_ext_io_num=PIN_NUM_CS,           // external CS pin
		.flags=LB_SPI_DEVICE_HALFDUPLEX,        // ALWAYS SET  to HALF DUPLEX MODE!! for display spi
    };

    // ====================================================================================================================


    vTaskDelay(500 / portTICK_RATE_MS);
	printf("\r\n==============================\r\n");
    printf("TFT display DEMO, LoBo 11/2017\r\n");
	printf("==============================\r\n");
    printf("Pins used: miso=%d, mosi=%d, sck=%d, cs=%d\r\n", PIN_NUM_MISO, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS);
	printf("==============================\r\n\r\n");

	// ==================================================================
	// ==== Initialize the SPI bus and attach the LCD to the SPI bus ====

	ret=spi_lobo_bus_add_device(SPI_BUS, &buscfg, &devcfg, &spi);
    assert(ret==ESP_OK);
	printf("SPI: display device added to spi bus (%d)\r\n", SPI_BUS);
	tft_disp_spi = spi;

	// ==== Test select/deselect ====
	ret = spi_lobo_device_select(spi, 1);
    assert(ret==ESP_OK);
	ret = spi_lobo_device_deselect(spi);
    assert(ret==ESP_OK);

	printf("SPI: attached display device, speed=%u\r\n", spi_lobo_get_speed(spi));
	printf("SPI: bus uses native pins: %s\r\n", spi_lobo_uses_native_pins(spi) ? "true" : "false");


	// ================================
	// ==== Initialize the Display ====

	printf("SPI: display init...\r\n");
	TFT_display_init();
    printf("OK\r\n");
	
	// ---- Detect maximum read speed ----
	tft_max_rdclock = find_rd_speed();
	printf("SPI: Max rd speed = %u\r\n", tft_max_rdclock);

    // ==== Set SPI clock used for display operations ====
	spi_lobo_set_speed(spi, DEFAULT_SPI_CLOCK);
	printf("SPI: Changed speed to %u\r\n", spi_lobo_get_speed(spi));

    printf("\r\n---------------------\r\n");
	printf("Graphics demo started\r\n");
	printf("---------------------\r\n");

	tft_font_rotate = 0;
	tft_text_wrap = 0;
	tft_font_transparent = 0;
	tft_font_forceFixed = 0;
	tft_gray_scale = 0;
	TFT_setGammaCurve(DEFAULT_GAMMA_CURVE);
	TFT_setRotation( DEFAULT_TFT_ORIENTATION );
	TFT_setFont(DEFAULT_FONT, NULL);
	TFT_resetclipwin();

    // ==== Initialize the file system ====
    
    const esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = "storage", // NULL
      .max_files = 5,
      .format_if_mount_failed = false
    };
    
    
    printf("\r\n\n");
	esp_vfs_spiffs_register(&conf);
    if (!esp_spiffs_mounted(conf.partition_label)) {
    	tft_fg = TFT_RED;
    	TFT_print("SPIFFS not mounted !", CENTER, LASTY+TFT_getfontheight()+2);
    }
    else {
    	tft_fg = TFT_GREEN;
    	TFT_print("SPIFFS Mounted.", CENTER, LASTY+TFT_getfontheight()+2);
    }
}

void tft_demo(){
	
	uint8_t disp_rot = LANDSCAPE_FLIP;
	TFT_setRotation(disp_rot);
    TS_setRotation( disp_rot );
	
	TFT_fillWindow(TFT_ORANGE);
	
	TFT_fillRect(10, 10, tft_dispWin.x2 - 20, tft_dispWin.y2 - 20, TFT_WHITE);
	
	char *tokyo = "TOKYO";
	char *year = "2020";
	tft_font_transparent = 1;
	tft_fg = TFT_NAVY;
	
	TFT_setFont(DEJAVU24_FONT, "/tools/compiled_font_files/dejavu.fon");
	
	TFT_print(tokyo, 115, 35);
	TFT_print(year, 130, 185);
	
	int startx = 90;
	int xgap = 35;
	int topy = 100;
	int bottomy = 130;
	
	TFT_drawArc(startx, topy, 30, 5, 170, 165, TFT_BLUE, TFT_BLUE);
	
	TFT_drawArc(startx + xgap, bottomy, 30, 5, 347, 85, TFT_YELLOW, TFT_YELLOW);
	TFT_drawArc(startx + xgap, bottomy, 30, 5, 90, 337, TFT_YELLOW, TFT_YELLOW);
	
	TFT_drawArc(startx + (xgap << 1), topy, 30, 5, 267, 165, TFT_BLACK, TFT_BLACK);
	TFT_drawArc(startx + (xgap << 1), topy, 30, 5, 170, 257, TFT_BLACK, TFT_BLACK);
	
	TFT_drawArc(startx + (xgap * 3), bottomy, 30, 5, 347, 85, TFT_GREEN, TFT_GREEN);
	TFT_drawArc(startx + (xgap * 3), bottomy, 30, 5, 90, 337, TFT_GREEN, TFT_GREEN);
	
	TFT_drawArc(startx + (xgap << 2), topy, 30, 5, 267, 257, TFT_RED, TFT_RED);
}
