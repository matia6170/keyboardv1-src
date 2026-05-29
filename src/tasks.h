#ifndef TASKS_H
#define TASKS_H

#include "Arduino.h"
#include "config.h"
#include "KeyMatrix.h"
#include "logo_bmp.h"
#include <Adafruit_SSD1306.h>
#include "interrupts.h"
#include "ESP32Encoder.h"
#include <RTClib.h>

int encoderValue = 0;                       // Move 10 pixels per frame
DateTime now2;

struct task_params {
    KeyMatrix<ROWS, COLS> *keypad;
    ESP32Encoder *encoder;
    RTC_DS3231 *rtc;
    Adafruit_SSD1306 *display;
    task_params(KeyMatrix<ROWS, COLS> *kp, ESP32Encoder *enc, RTC_DS3231 *rt, Adafruit_SSD1306 *disp) : keypad(kp), encoder(enc), rtc(rt), display(disp) {}
};

void heartbeat_task(void *pvParameters) {
    task_params *params = (task_params *)pvParameters;
    RTC_DS3231 *rtc = params->rtc;
#ifdef DEBUG
    Serial.println("Heartbeat task started.");
#endif

  while (true) {
    // now2 = rtc->now();
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 1 second
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 1 second
  }
}


void keyboard_task(void *pvParameters) {
    task_params *params = (task_params *)pvParameters;
    KeyMatrix<ROWS, COLS> *keypad = params->keypad;
    ESP32Encoder *encoder = params->encoder;
#ifdef DEBUG
    Serial.println("Keyboard task started.");
#endif
    long lastCount = 0;

    while (true) {
        keypad->scan_keys(); 

         if (encoderUpdated) {
            encoderUpdated = false;
            long count = encoder->getCount();
            if (count != lastCount) {
                lastCount = count;
                // update display here
                encoderValue = count; // Example: Change speed based on encoder count
                Serial.printf("Encoder count: %ld\n", count);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Adjust the delay as needed for responsiveness
    }
}

void display_task(void *pvParameters) {
    task_params *params = (task_params *)pvParameters;
    Adafruit_SSD1306 *display = params->display;
#ifdef DEBUG
    Serial.println("Display task started.");
#endif
    display->clearDisplay();
    display->drawBitmap(0, 0, logo_bmp, 128, 64, WHITE);
    display->display();
    vTaskDelay(pdMS_TO_TICKS(2000)); // show logo

    // int counter = 0;
    // while (true) {
    //     // Update display content here if needed
    //         display->clearDisplay();
    //         display->setTextSize(1);
    //         display->setTextColor(SSD1306_WHITE);
    //         display->setCursor(0,0);
    //         display->println(F("KeyboardV1"));
    //         display->setTextSize(1);
    //         display->println(F("---------------------"));
    //         display->println(F("Only 2 data pins!"));
    //         display->println();
    //         display->print(F("Counter: "));
    //         display->println(counter++);
    //         display->display();
    //     vTaskDelay(pdMS_TO_TICKS(1000)); // Update every second (adjust
    // }
    // Animation Variables
    const int FPS = 24;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / FPS); // ~41 ticks
    TickType_t xLastWakeTime = xTaskGetTickCount();

    int boxSize = 10;
    int xPos = 0;                            // Start at left edge
    int yPos = (64 - boxSize) / 2;           // Center vertically on a 64px tall screen
    int xVelocity = 0;                       // Move 3 pixels per frame

    while (true) {
        if (xVelocity < 0) xVelocity = -1 * abs(encoderValue);
        else xVelocity = abs(encoderValue);

        // 1. Clear the previous frame
        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(SSD1306_WHITE);
        display->setCursor(0,0);

  // Print the date and time parameters
        DateTime now = params->rtc->now();
        display->printf("%04d/%02d/%02d (%s) %02d:%02d:%02d\n", 
                now.year(), now.month(), now.day(), 
                daysOfTheWeek[now.dayOfTheWeek()],
                now.hour(), now.minute(), now.second());
        display->setCursor(0,50);
        display->printf("Speed: %d", abs(encoderValue));

        // 2. Draw the box at the new coordinates
        display->fillRect(xPos, yPos, boxSize, boxSize, WHITE);

        // 3. Push the buffer to the OLED via I2C
        display->display();

        // 4. Calculate the next position
        xPos += xVelocity;

        // 5. Check boundary collisions (128px width)
        if (xPos <= 0) {
            xPos = 0;                        // Snap to left edge
            xVelocity = -xVelocity;          // Reverse direction (move right)
        } 
        else if (xPos >= (128 - boxSize)) {
            xPos = 128 - boxSize;            // Snap to right edge
            xVelocity = -xVelocity;          // Reverse direction (move left)
        }

        // 6. Block task until precisely 41ms have passed since last wake
        vTaskDelayUntil(&xLastWakeTime, xFrequency); 
    }
}





#endif // TASKS_H