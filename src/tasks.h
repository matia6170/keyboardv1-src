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
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

int encoderValue = 0;                       // Move 10 pixels per frame
DateTime now;
portMUX_TYPE nowMux = portMUX_INITIALIZER_UNLOCKED;


struct task_params {
    KeyMatrix<ROWS, COLS> *keypad;
    ESP32Encoder *encoder;
    RTC_DS3231 *rtc;
    Adafruit_SSD1306 *display;
    SemaphoreHandle_t *i2cMutex;
    task_params(KeyMatrix<ROWS, COLS> *kp, ESP32Encoder *enc, RTC_DS3231 *rt, Adafruit_SSD1306 *disp, SemaphoreHandle_t *mutex) : keypad(kp), encoder(enc), rtc(rt), display(disp), i2cMutex(mutex) {}
};

void fetch_time_from_rtc_task(void *pvParameters) {
    task_params *params = (task_params *)pvParameters;
    RTC_DS3231 *rtc = params->rtc;

    while (true) {
        DateTime temp;

        if (xSemaphoreTake(*(params->i2cMutex), portMAX_DELAY) == pdTRUE) {
            temp = rtc->now();
            xSemaphoreGive(*(params->i2cMutex));
        }

        //write protect
        //display task will now read 'now' after being fully written by this task. after unblock.
        portENTER_CRITICAL(&nowMux);
        now = temp;
        portEXIT_CRITICAL(&nowMux);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Update every second
    }
}

void heartbeat_task(void *pvParameters) {
#ifdef DEBUG
    Serial.println("Heartbeat task started.");
#endif

  while (true) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 1 second
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 1 second
  }
}

void io_task(void *pvParameters) {
    task_params *params = (task_params *)pvParameters;
#ifdef DEBUG
    Serial.println("IO task started.");
#endif  

    while(true){
        // Example: Read digital button value and print it
        int buttonValue = digitalRead(BOOT_BTN_PIN);
        // Serial.printf("Boot Button Value: %d\n", buttonValue);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust the delay as needed

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
                tone(BEEPER_PIN, 1000, 50); // Beep at 1kHz for 50ms
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
    if (xSemaphoreTake(*(params->i2cMutex), portMAX_DELAY) == pdTRUE) {
        display->display();
        xSemaphoreGive(*(params->i2cMutex));
    }
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
        DateTime currentTime;
        //read protect
        portENTER_CRITICAL(&nowMux);
        currentTime = now;
        portEXIT_CRITICAL(&nowMux);        
        display->printf("%04d/%02d/%02d (%s) %02d:%02d:%02d\n", 
                currentTime.year(), currentTime.month(), currentTime.day(), 
                daysOfTheWeek[currentTime.dayOfTheWeek()],
                currentTime.hour(), currentTime.minute(), currentTime.second());
        display->setCursor(0,50);
        display->printf("Speed: %d", abs(encoderValue));

        // 2. Draw the box at the new coordinates
        display->fillRect(xPos, yPos, boxSize, boxSize, WHITE);

        // 3. Push the buffer to the OLED via I2C
        if (xSemaphoreTake(*(params->i2cMutex), portMAX_DELAY) == pdTRUE) {
            display->display();
            xSemaphoreGive(*(params->i2cMutex));
        }

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