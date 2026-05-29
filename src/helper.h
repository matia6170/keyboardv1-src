#ifndef HELPER_H
#define HELPER_H

#include "Arduino.h"
#include "Wire.h"


void scan_i2c(){
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning for I2C devices...");

  // I2C addresses range from 1 to 127
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Wire.endTransmission to see if
    // a device acknowledged the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("-> Found device at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      
      // Common address identification helper
      if (address == 0x3C || address == 0x3D) {
        Serial.println(" (Likely SSD1306/SH1106 OLED Display)");
      } else {
        Serial.println();
      }

      nDevices++;
    } 
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found.\n");
  } else {
    Serial.println("Scan complete.\n");
  }



}


#endif // HELPER_H