#include <Arduino.h>
#include <Ultrasonic.h>
#define HC_PIN_0_0 3
#define HC_PIN_0_1 5
#define HC_PIN_1_0 6
#define HC_PIN_1_1 7
#define HC_PIN_2_0 8
#define HC_PIN_2_1 9
#define HC_PIN_3_0 10
#define HC_PIN_3_1 11
#define HC_PIN_4_0 12
#define HC_PIN_4_1 13
#define OOB(a) ((a)>200?0:(a))//Out of bounds returns 0 if out of bounds
#define INT_DIV_2(a) ((a)>>1)
#define NUM_DIST_SENS 5
#define PWM_PIN_1 9 // Predefined PWM output pin
#define PWM_PIN_2 10
#define DIRECTION_PIN_1 2 // Predefined pin for controlling direction
#define DIRECTION_PIN_2 4
#define STOP_CHAR_RX 101
#define STOP_CHAR_TX 201
#define LIST_SIZE 3

int8_t buffer[LIST_SIZE]; // String to store incoming data


Ultrasonic distanceSensor0(HC_PIN_0_0, HC_PIN_0_1); 
Ultrasonic distanceSensor1(HC_PIN_1_0, HC_PIN_1_1);
Ultrasonic distanceSensor2(HC_PIN_2_0, HC_PIN_2_1);
Ultrasonic distanceSensor3(HC_PIN_3_0, HC_PIN_3_1);
Ultrasonic distanceSensor4(HC_PIN_4_0, HC_PIN_4_1);

void setup()
{
	pinMode(PWM_PIN_1, OUTPUT);
	pinMode(DIRECTION_PIN_1, OUTPUT);
	pinMode(PWM_PIN_2, OUTPUT);
	pinMode(DIRECTION_PIN_2, OUTPUT);
	Serial.begin(9600); // Initialize serial communication
}

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
	if (left){
		analogWrite(PWM_PIN_1, absValue);
	}else{
		analogWrite(PWM_PIN_2, absValue);
	}
}

bool processBuffer()
{
	// Extract last two characters separated by STOP_CHAR
	// Extract last two characters
	uint8_t index;
	bool found = false;
	for (uint8_t i =0; i<LIST_SIZE; i++){
		if (buffer[i]==STOP_CHAR_RX){
			index = i;
			found = true;
			continue;
		}
	}

	if (found)
	{
		switch (index)
		{
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
	return found;
}

uint8_t * readSensors(){
	uint8_t *sensorArray = new uint8_t[NUM_DIST_SENS];
	uint8_t sensorValue0 = INT_DIV_2(distanceSensor0.read(CM)); // divides the number by 2
	uint8_t sensorValue1 = INT_DIV_2(distanceSensor1.read(CM));
	uint8_t sensorValue2 = INT_DIV_2(distanceSensor2.read(CM));
	uint8_t sensorValue3 = INT_DIV_2(distanceSensor3.read(CM));
	uint8_t sensorValue4 = INT_DIV_2(distanceSensor4.read(CM));
	sensorArray[0] = OOB(sensorValue0); //cuts off the number at 200
	sensorArray[1] = OOB(sensorValue1);
	sensorArray[2] = OOB(sensorValue2);
	sensorArray[3] = OOB(sensorValue3);
	sensorArray[4] = OOB(sensorValue4);
	return sensorArray;
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
		processBuffer();
	}
	uint8_t * sensorArray = readSensors();
	for(uint8_t i =0; i<NUM_DIST_SENS; i++){
		Serial.print(sensorArray[i]);
	}
	Serial.print(STOP_CHAR_TX);

	// DO NOT DELETE THIS LINE
	delete[] sensorArray;
}
