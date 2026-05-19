#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "KeyMatrix.h"
#include "M5Unified.h"

#include "USBMSC.h"


#define ROWS 4
#define COLS 3

const int rowPins[ROWS] = {3, 4, 6, 40};
const int colPins[COLS] = {5, 39, 14};

char keyMap[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {' ', '0', '1'}
};

KeyMatrix<ROWS, COLS> Keypad(rowPins, colPins);
USBHIDKeyboard Keyboard;
USBMSC msc;


bool blue;
char curr_key;
void key_evt_callback(int row, int col, bool down, char key) { 
  Serial.printf("Callback: %c (%d, %d) %s\n", key, col, row, down ? "DOWN" : "UP");
  if (down){
    Keyboard.press(key);
    curr_key = key;
  }
  else{
    Keyboard.release(key);
  }

  blue = down;
  
}


void setup() {
    // put your setup code here, to run once:
    // 1. Mandatory for Mode 0 Keyboard

    // disc 
  msc.vendorID("M5Stack");
  msc.productID("StampS3-Disk");
  msc.begin(512, 100); // 512 bytes per block, 100 blocks total (example)
    
  
  blue = false;
  curr_key = 0;
  // inits hid keyboard
    Keyboard.begin();

    // gets translated in to usbserial when using tiny usb
    Serial.begin();
    
  //this line needs to be called at the end. 
  // Initializes (upto 6) devices
    USB.begin();

    // USBSerial.begin(115200);
    
    Keypad.init();
    Keypad.register_callback(key_evt_callback);
    Keypad.init_key_map(keyMap);

    auto cfg = M5.config();
    M5.begin(cfg);
    int textsize = M5.Display.height() / 60;
    if (textsize == 0) {
        textsize = 1;
    }
    M5.Display.setTextSize(textsize);
    // Set cursor position (x, y)
    M5.Display.setCursor(10, 0);

    // Print text
    M5.Display.print("ARDUINO_USB_MODE: ");
    M5.Display.println(ARDUINO_USB_MODE);
    M5.Display.println(textsize);


  }

void loop() { 
  Keypad.scan_keys(); 
  M5.Display.setCursor(10, 20);
  M5.Display.print(curr_key);
  
  M5.Display.fillRect(70, 50, 50, 50, blue?BLUE:RED);

  M5.update();

}
