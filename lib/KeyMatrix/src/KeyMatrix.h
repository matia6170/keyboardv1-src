
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

   public:
    KeyMatrix(const int (&rowPins)[ROWS], const int (&colPins)[COLS]);
    void init();
    void scan_keys();
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
}

template <size_t ROWS, size_t COLS>
void KeyMatrix<ROWS, COLS>::scan_keys() {
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
                Serial.printf("(%d, %d) DOWN\n", c, r);
                // Update the state
                _lastKeyState[r][c] = true;
            }
            // Key up
            else if (currentKey == LOW && _lastKeyState[r][c] == true) {
                // Serial.print("Key UP:   ");
                // Serial.println(keyMap[r][c]);
                Serial.printf("(%d, %d) UP\n", c, r);
                // Update the state
                _lastKeyState[r][c] = false;
            }
        }

        // set current col to 0
        digitalWrite(_colPins[c], LOW);
    }
}

#endif