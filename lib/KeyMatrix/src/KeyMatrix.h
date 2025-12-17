
#ifndef KeyMatrix_H
#define KeyMatrix_H

#include <Arduino.h>

typedef unsigned int size_t;

using namespace std;

template <size_t ROWS, size_t COLS>
class KeyMatrix {
   private:
    int _rowPins[ROWS];
    int _colPins[COLS];
    bool _lastKeyState[ROWS][COLS];
    char _keyMap[ROWS][COLS];

    void (*_key_evt_callback)(int row, int col, bool down, char key) = nullptr;

    const int DEBOUNCE_DURATION_MS = 5;
    long lastScanTime = 0;

   public:
    KeyMatrix(const int (&rowPins)[ROWS], const int (&colPins)[COLS]);
    void init();
    void init_key_map(char (&keymap)[ROWS][COLS]);
    void scan_keys();
    void register_callback(void (*key_evt_callback)(int row, int col, bool down, char key));
};

template <size_t ROWS, size_t COLS>
KeyMatrix<ROWS, COLS>::KeyMatrix(const int (&rowPins)[ROWS], const int (&colPins)[COLS]) {
    // copy pin numbres
    for (size_t i = 0; i < ROWS; i++) _rowPins[i] = rowPins[i];
    for (size_t i = 0; i < COLS; i++) _colPins[i] = colPins[i];

    // init last key state
    for (size_t i = 0; i < ROWS; i++)
        for (size_t j = 0; j < COLS; j++) _lastKeyState[i][j] = false;
}

template <size_t ROWS, size_t COLS>
void KeyMatrix<ROWS, COLS>::init() {
    // 1. Initialize Column Pins as Outputs
    for (int j = 0; j < COLS; j++) {
        pinMode(_colPins[j], OUTPUT);
        // Set all columns LOW initially to ensure no current flows
        digitalWrite(_colPins[j], LOW);
    }

    // 2. Initialize Row Pins as Inputs
    for (int i = 0; i < ROWS; i++) {
        pinMode(_rowPins[i], INPUT);
    }
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) _keyMap[r][c] = 0x00;

}

template <size_t ROWS, size_t COLS>
void KeyMatrix<ROWS, COLS>::init_key_map(char (&keymap)[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) _keyMap[r][c] = keymap[r][c];
}

template <size_t ROWS, size_t COLS>
void KeyMatrix<ROWS, COLS>::scan_keys() {
    if (millis() - lastScanTime > DEBOUNCE_DURATION_MS) {
        // Iterate through each column (Driver)
        for (int c = 0; c < COLS; c++) {
            // select current col
            digitalWrite(_colPins[c], HIGH);

            // Iterate through each row (Sensor)
            for (int r = 0; r < ROWS; r++) {
                bool currentKey = digitalRead(_rowPins[r]);

                // Key down
                if (currentKey == HIGH && _lastKeyState[r][c] == false) {
                    // Serial.print("Key DOWN: ");
                    // Serial.println(keyMap[r][c]);
                    if (_key_evt_callback != nullptr) _key_evt_callback(r, c, true, _keyMap[r][c]);
                    // Serial.printf("(%d, %d) DOWN\n", c, r);
                    // Update the state
                    _lastKeyState[r][c] = true;
                }
                // Key up
                else if (currentKey == LOW && _lastKeyState[r][c] == true) {
                    if (_key_evt_callback != nullptr) _key_evt_callback(r, c, false, _keyMap[r][c]);
                    // Serial.printf("(%d, %d) UP\n", c, r);
                    // Update the state
                    _lastKeyState[r][c] = false;
                }
            }

            // set current col to 0
            digitalWrite(_colPins[c], LOW);
        }
        lastScanTime = millis();
    }
}

template <size_t ROWS, size_t COLS>
void KeyMatrix<ROWS, COLS>::register_callback(void (*key_evt_callback)(int row, int col, bool down, char key)) {
    _key_evt_callback = key_evt_callback;
}

#endif