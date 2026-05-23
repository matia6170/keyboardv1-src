#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "KeyMatrix.h"

// #include "USBMSC.h"

#include "KeyCode.h"
#include "KeySettings.h"

#define LED_PIN 1

KeyMatrix<ROWS, COLS> Keypad(rowPins, colPins);
USBHIDKeyboard Keyboard;
// USBMSC msc;


bool blue;
uint8_t curr_key;
void key_evt_callback(int row, int col, bool down, uint8_t key) { 
  Serial.printf("Callback: %c (%d, %d) %s\n", key, col, row, down ? "DOWN" : "UP");
  if (down){
    Keyboard.pressRaw(key);
    curr_key = key;
    digitalWrite(LED_PIN, HIGH);
  }
  else{
    Keyboard.releaseRaw(key);
    digitalWrite(LED_PIN, LOW);
  }

  // blue = down;
  
}


void setup() {
  //   // put your setup code here, to run once:
  //   // 1. Mandatory for Mode 0 Keyboard

  //   // disc 
  // msc.vendorID("M5Stack");
  // msc.productID("StampS3-Disk");
  // msc.begin(512, 100); // 512 bytes per block, 100 blocks total (example)
    
  
  // blue = false;
  // curr_key = 0;
  // // inits hid keyboard
    Keyboard.begin();

  //   // gets translated in to usbserial when using tiny usb
    Serial.begin(115200);
    
  // //this line needs to be called at the end. 
  // // Initializes (upto 6) devices
    USB.begin();

    
    Keypad.init();
    Keypad.register_callback(key_evt_callback);
    Keypad.init_key_map(keyMap);
  
  pinMode(LED_PIN, OUTPUT); 
}
void loop() { 
  Keypad.scan_keys(); 
  // Serial.println("Hello world");
  // digitalWrite(LED_PIN, HIGH);
  // delay(100);
  // digitalWrite(1, LOW);
  // delay(100);


}
