
#ifndef __BT_LCD_H__
#define __BT_LCD_H__



// updates the ui with current song title, artist and album
void bt_app_updateUI(int id, char* text);

// sets up the bluetooth
void setupBT();

// sets up the screen
void setupScreen();

// draws the olympics picture
void tft_demo();

#endif
