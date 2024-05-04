#include <Arduino.h>

#define STOP_CHAR 101
uint8_t i = 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    for (size_t j = 0; j < 5; j++) {
        i++;
        Serial.print(char(i));
    }
    Serial.print(char(STOP_CHAR));
}