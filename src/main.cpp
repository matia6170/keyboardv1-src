#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
// #include "USBMSC.h"

// Custom Headers
#include "KeyMatrix.h"
#include "config.h"
#include "tasks.h"
#include "helper.h"
#include "interrupts.h"

#include "logo_bmp.h"


// Libs
#include <ESP32Encoder.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;
SemaphoreHandle_t i2cMutex = nullptr;

KeyMatrix<ROWS, COLS> Keypad(rowPins, colPins);
ESP32Encoder encoder;

// USB HID Keyboard instance
USBHIDKeyboard Keyboard;

// USBMSC msc;



bool blue;
void key_evt_callback(int row, int col, bool down, uint8_t key) { 
#ifdef DEBUG
  Serial.printf("Callback: %c (%d, %d) %s\n", key, col, row, down ? "DOWN" : "UP");
#endif
  if (down){
    // Keyboard.pressRaw(key);
    // digitalWrite(LED_PIN, HIGH);
  }
  else{
    // Keyboard.releaseRaw(key);
    // digitalWrite(LED_PIN, LOW);
  }

  // blue = down;
  
}

void init_io(){
    pinMode(LED_PIN, OUTPUT); 
    pinMode(ENCODER_BTN, INPUT_PULLUP);
    pinMode(BOOT_BTN_PIN, INPUT_PULLUP);
    pinMode(CONTROL_BTN_PIN, INPUT_PULLUP);
    pinMode(BEEPER_PIN, OUTPUT);

  i2cMutex = xSemaphoreCreateMutex();
  if (i2cMutex == nullptr) {
    Serial.println("Failed to create I2C mutex.");
    while (true) delay(10);
  }

  //demo test take of mutex and never give it back to test blocking behavior of tasks that need to access i2c
//   xSemaphoreTake(i2cMutex, portMAX_DELAY);

    // Init Wire
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(400000); // Set I2C frequency to 400kHz for faster communication with peripherals

}
void init_peripherals(){

    // ============================
    // Init HID Keyboard
    Keyboard.begin();
    // ============================
    // INIT USB (must be called after initializing all devices). This will translate to usbserial if using tinyusb, but is still required to initialize the USB stack and dispatch events.
    USB.begin();

    // ============================
    // init key matrix
    // ============================
    Keypad.init();
    Keypad.register_callback(key_evt_callback);
    Keypad.init_key_map(keyMap);

    // ============================
    // init encoder
    // ============================
    encoder.attachSingleEdge(ENCODER_INA, ENCODER_INB);
    // Mechanical encoders often need a stronger PCNT glitch filter than the library default.
    encoder.setFilter(1023);
    // Clear the starting count position to 0
    encoder.setCount(0);
    Serial.println("Hardware Encoder Initialized.");
    attachInterrupt(digitalPinToInterrupt(ENCODER_INA), encoderISR, RISING);
    // attachInterrupt(digitalPinToInterrupt(ENCODER_INB), encoderISR, CHANGE);

    // ============================
    // Initialize the RTC chip
    // ============================
    if (!rtc.begin(&Wire)) {
        display.println("Couldn't find DS3231! Check your connections.");
        while (1) delay(10);
    }

    // Check if the RTC lost power (e.g., the coin cell battery was removed/drained)
    if (rtc.lostPower()) {
        display.println("RTC lost power, let's set the time!");
        
        // This line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        
        // To set a specific manual date/time, use this instead:
        // rtc.adjust(DateTime(2026, 5, 27, 23, 59, 0)); // YYYY, MM, DD, HH, MM, SS
    }

    // ============================
    // Initialize the OLED display
    // ============================
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed. Check address/wiring!"));
        for(;;);
    }

    display.clearDisplay();
    // display.setTextSize(1);
    // display.setTextColor(SSD1306_WHITE);
    // display.setCursor(0,0);
    // display.println(F("KeyboardV1"));
    // display.setTextSize(1);
    // display.println(F("---------------------"));
    // display.println(F("Only 2 data pins!"));
    // display.drawBitmap(0, 0, logo_bmp, 128, 64, WHITE);
    // display.display();

}
void init_tasks(){
  static task_params params(&Keypad, &encoder, &rtc, &display, &i2cMutex);
  xTaskCreate(heartbeat_task, "heartbeat", 1024, &params, 1, NULL);
  xTaskCreate(fetch_time_from_rtc_task, "fetch_time", 4096, &params, 1, NULL);
  xTaskCreate(keyboard_task, "keyboard", 4096, &params, 1, NULL);
  xTaskCreate(display_task, "display", 8192, &params, 1, NULL);
//   xTaskCreate(io_task, "analog_btn", 4096, &params, 1, NULL);
}

void setup() {
    Serial.begin(115200);
    init_io();
    init_peripherals();
    init_tasks();

  //   // put your setup code here, to run once:
  //   // 1. Mandatory for Mode 0 Keyboard

  //   // disc 
  // msc.vendorID("M5Stack");
  // msc.productID("StampS3-Disk");
  // msc.begin(512, 100); // 512 bytes per block, 100 blocks total (example)
    
  
  // blue = false;
  // curr_key = 0;
  // // inits hid keyboard
    // Keyboard.begin();

  //   // gets translated in to usbserial when using tiny usb
    
  // //this line needs to be called at the end. 
  // // Initializes (upto 6) devices
    // USB.begin();

    
    // Keypad.init();
    // Keypad.register_callback(key_evt_callback);
    // Keypad.init_key_map(keyMap);
  
//   pinMode(LED_PIN, OUTPUT); 


  // // Initialize custom I2C pins on the ESP32-S3
//   Wire.begin(I2C_SDA, I2C_SCL);






//   display.display();
}
void loop() { 
  // Keypad.scan_keys(); 


  // Serial.println("Hello world");
//   digitalWrite(LED_PIN, HIGH);
//   delay(100);
//   digitalWrite(1, LOW);
//   delay(100);

//   // scan_i2c();
//   // delay(1000);
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0,0);
//   // Fetch the current time snapshot from the DS3231
//   DateTime now = rtc.now();

//   // Print the date and time parameters
//   display.printf("%04d/%02d/%02d (%s) %02d:%02d:%02d\n", 
//                 now.year(), now.month(), now.day(), 
//                 daysOfTheWeek[now.dayOfTheWeek()],
//                 now.hour(), now.minute(), now.second());

//   // The DS3231 has a built-in temperature compensated crystal oscillator (TCXO).
//   // We can read that internal temperature sensor!
//   display.printf("Temperature: %.2f°C\n", rtc.getTemperature());
//   display.println(F("---------------------"));
//   display.display();

//   delay(3000); // Poll every 3 seconds
//   scan_i2c();

}
