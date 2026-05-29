#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "Arduino.h"

volatile bool encoderUpdated = false;
void IRAM_ATTR encoderISR() {
    encoderUpdated = true;
}
#endif // INTERRUPTS_H