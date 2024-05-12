#include <Arduino.h>


#define STOP_CHAR 101
#define LIST_SIZE 3
#define NUM_DIST_SENSORS 5
int8_t buffer[LIST_SIZE];
unsigned long intervalTime;

void setup() {
    Serial.begin(9600);
	intervalTime = millis();
}

void loop() {
	if (millis() - intervalTime > 3000){
		Serial.println("three sec");
		intervalTime = millis();
	}

}
