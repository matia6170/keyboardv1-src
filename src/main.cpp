#include <Arduino.h>
#include "KeyMatrix.h"

#define ROWS 4
#define COLS 3

const int rowPins[ROWS] = {3, 4, 6, 40};
const int colPins[COLS] = {5, 39, 14};

KeyMatrix<ROWS,COLS> Keypad(rowPins, colPins);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Keypad.init();

}

void loop() {
  Keypad.scan_keys();
}
~