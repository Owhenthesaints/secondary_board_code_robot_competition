#include <Arduino.h>

#define STOP_CHAR 101
#define LIST_SIZE 3
#define NUM_DIST_SENSORS 5
int8_t buffer[LIST_SIZE];

void setup() {
    Serial.begin(9600);
}

void loop()
{
	while (Serial.available() > 3)
	{
		Serial.read();
	}
	uint8_t incremental_pointer(0);
	while (incremental_pointer <= 2)
	{
		if (Serial.available())
		{
			char incommingChar = Serial.read();
			buffer[incremental_pointer] = int8_t(incommingChar);
			incremental_pointer++;
		}
	}
	for (size_t i(0); i < NUM_DIST_SENSORS; i++){
		Serial.print(char(buffer[i%2]));
	}
	Serial.print(char(STOP_CHAR));
}