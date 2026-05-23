#ifndef KEYCODE_H 
#define KEYCODE_H

#include <Arduino.h>
#include "USBHIDKeyboard.h" // Pulls in standard definitions like KEY_LEFT_CTRL, KEY_ESC, etc.

// --- Layout Matrix Size ---
#define ROWS 6
#define COLS 14

// Matrix array accessible in main code execution
extern uint8_t keyMap[ROWS][COLS];

#endif 