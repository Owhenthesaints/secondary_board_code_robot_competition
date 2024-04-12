#include <Arduino.h>
#define PWM_PIN_1 9 // Predefined PWM output pin
#define PWM_PIN_2 10
#define DIRECTION_PIN_1 2 // Predefined pin for controlling direction
#define DIRECTION_PIN_2 4
#define STOP_CHAR 101
#define LIST_SIZE 3

int8_t buffer[LIST_SIZE]; // String to store incoming data

void writeToMotor(bool left, int8_t inputValue)
{
	// Calculate absolute value of input
	int8_t absValue = abs(inputValue);

	// Determine direction
	if (inputValue < 0)
	{
		// Set direction pin HIGH for negative direction
		if (left)
			digitalWrite(DIRECTION_PIN_1, HIGH);
		else
			digitalWrite(DIRECTION_PIN_2, HIGH);
	}
	else
	{
		// Set direction pin LOW for positive direction
		if (left)
			digitalWrite(DIRECTION_PIN_1, LOW);
		else
			digitalWrite(DIRECTION_PIN_2, LOW);
	}

	// Convert absolute char value to PWM value (0 to 100 mapped to 0 to 255)

	// Output PWM value
	if (left)
		analogWrite(PWM_PIN_1, 10);
	else
		analogWrite(PWM_PIN_2, 10);
}

void processBuffer()
{
	// Extract last two characters separated by STOP_CHAR
	// Extract last two characters
	uint8_t index;
	for (uint8_t i =0; i++; i<LIST_SIZE){
		if (buffer[i]==STOP_CHAR){
			index = i;
			continue;
		}
	}

	switch (index){
	case 0:
		writeToMotor(true, buffer[1]);
		writeToMotor(false, buffer[2]);
		break;
	case 1:
		writeToMotor(false, buffer[0]);
		writeToMotor(true, buffer[2]);
		break;
	case 2:
		writeToMotor(true, buffer[0]);
		writeToMotor(false, buffer[1]);
		break;
	default:
		Serial.println("inv");
		break;
	}
}

void setup()
{
	pinMode(PWM_PIN_1, OUTPUT);
	pinMode(DIRECTION_PIN_1, OUTPUT);
	pinMode(PWM_PIN_2, OUTPUT);
	pinMode(DIRECTION_PIN_2, OUTPUT);
	Serial.begin(9600); // Initialize serial communication
}

void loop()
{
	uint8_t incrementalPointer = 0;
	while (incrementalPointer <= 2)
	{
		if (Serial.available()){
			char incomingChar = Serial.read();
			buffer[incrementalPointer] = int8_t(incomingChar);
			incrementalPointer++;
		}
	}
	if (incrementalPointer==3)
	{
		Serial.println(String(buffer[0]));
		processBuffer();
	}
}
