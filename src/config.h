#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "USBHIDKeyboard.h" // Pulls in standard definitions like KEY_LEFT_CTRL, KEY_ESC, etc.
#include "class/hid/hid.h"

#define DEBUG

/*
* Keyboard Matrix Configuration and Key Mapping
*/

// --- Layout Matrix Size ---
#define ROWS 6
#define COLS 14

// Matrix array accessible in main code execution
extern uint8_t keyMap[ROWS][COLS];


// ESP32-S3 Pin definitions traced from schematic
const int rowPins[ROWS] = {
  12, // row0 -> GPIO12
  13, // row1 -> GPIO13
  14, // row2 -> GPIO14
  21, // row3 -> GPIO21
  45, // row4 -> GPIO45
  38  // row5 -> GPIO38
};

const int colPins[COLS] = {
   4, // col0  -> GPIO4
   5, // col1  -> GPIO5
   6, // col2  -> GPIO6
   7, // col3  -> GPIO7
  15, // col4  -> GPIO15
  16, // col5  -> GPIO16
  17, // col6  -> GPIO17
  18, // col7  -> GPIO18
   8, // col8  -> GPIO8
   3, // col9  -> GPIO3
  46, // col10 -> GPIO46
   9, // col11 -> GPIO9
  10, // col12 -> GPIO10
  11  // col13 -> GPIO11
};

// Updated keyMap matching standard TinyUSB HID definitions
uint8_t keyMap[ROWS][COLS] = {
  // Row 0
  {
    HID_KEY_ESCAPE, HID_KEY_F1, HID_KEY_F2, HID_KEY_F3, HID_KEY_F4, 
    HID_KEY_F5, HID_KEY_F6, HID_KEY_F7, HID_KEY_F8, HID_KEY_F9, 
    HID_KEY_F10, HID_KEY_F11, HID_KEY_F12, HID_KEY_PRINT_SCREEN
  },
  
  // Row 1
  {
    HID_KEY_GRAVE, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, 
    HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, HID_KEY_9, 
    HID_KEY_0, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_BACKSPACE
  },
  
  // Row 2
  {
    HID_KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, 
    HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, 
    HID_KEY_P, HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT, HID_KEY_BACKSLASH
  },
  
  // Row 3
  {
    HID_KEY_CAPS_LOCK, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, 
    HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K, HID_KEY_L, 
    HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, HID_KEY_ENTER, HID_KEY_PAGE_DOWN
  },
  
  // Row 4
  {
    HID_KEY_SHIFT_LEFT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, 
    HID_KEY_B, HID_KEY_N, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, 
    HID_KEY_SLASH, HID_KEY_SHIFT_RIGHT, HID_KEY_ARROW_UP, HID_KEY_END
  },
  
  // Row 5 (0x00 is used for internal firmware/Fn layer logic)
  {
    HID_KEY_CONTROL_LEFT, HID_KEY_GUI_LEFT, HID_KEY_ALT_LEFT, HID_KEY_SPACE, 
    HID_KEY_ALT_RIGHT, HID_KEY_1, HID_KEY_CONTROL_RIGHT, HID_KEY_ARROW_LEFT, 
    HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT, HID_KEY_2, HID_KEY_HOME, 
    HID_KEY_PAGE_UP, HID_KEY_DELETE 
  }
};

/*
* Other Peripheral Configurations (OLED, RTC, Encoder) and Global Variables
*/


//encoder
  #define ENCODER_INA 47
  #define ENCODER_INB 48
  #define ENCODER_BTN 2
  const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//oled
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64

  // Define your custom I2C pins
  #define I2C_SDA 42
  #define I2C_SCL 41

  // For I2C, standard breakouts usually live at address 0x3C
  #define OLED_RESET    -1 // -1 means sharing the microcontroller reset pin
  #define SCREEN_ADDRESS 0x3C

//rtc
  //#define DS1307_ADDRESS 0x68 // I2C address for DS1307 RTC (not used directly since RTClib handles it)

// Heartbeat LED pin
  #define LED_PIN 1


/*
Scanning for I2C devices...
-> Found device at address 0x3C (Likely SSD1306/SH1106 OLED Display)
-> Found device at address 0x57
-> Found device at address 0x68
*/
#endif // CONFIG_H