#include <Arduino.h>

#define STOP_CHAR 101
#define LIST_SIZE 3
#define NUM_DIST_SENSORS 5
#define MAX_TIME 1000
int8_t buffer[LIST_SIZE];

inline bool TimeExceeded(long int & intervalTime){
	return static_cast<long int>(intervalTime + MAX_TIME - millis()) < 0;
}

void setup() {
    Serial.begin(9600);
	delay(1000);
}

void loop()
{
	static long int intervalTime(millis());
	while (Serial.available() > 3)
	{
		Serial.read();
	}
	intervalTime = millis();
	uint8_t incremental_pointer(0);
	while (incremental_pointer <= 2)
	{
		if (TimeExceeded(intervalTime)){
			break;
		}
		else if (Serial.available())
		{
			char incommingChar = Serial.read();
			buffer[incremental_pointer] = int8_t(incommingChar);
			incremental_pointer++;
		}
	}

	for (size_t i(0); i < NUM_DIST_SENSORS; i++){
		Serial.print(4);
	}
	Serial.print(char(STOP_CHAR));
}