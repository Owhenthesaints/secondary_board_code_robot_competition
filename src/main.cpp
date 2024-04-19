#include <Arduino.h>
#include <Ultrasonic.h>
#include <math.h>
#define HC_PIN_0_0 30
#define HC_PIN_0_1 31
#define HC_PIN_1_0 32
#define HC_PIN_1_1 33
#define HC_PIN_2_0 34
#define HC_PIN_2_1 35
#define HC_PIN_3_0 36
#define HC_PIN_3_1 37
#define HC_PIN_4_0 38
#define HC_PIN_4_1 39
#define OOB(a) ((a)>200?0:(a))//Out of bounds returns 0 if out of bounds
#define UINT_DIV_2(a) ((a)>>1)
#define NUM_DIST_SENS 5
#define PWM_PIN_1 2 // Predefined PWM output pin
#define PWM_PIN_2 3
#define ENABLE_PIN_1 22
#define ENABLE_PIN_2 24
#define DIRECTION_PIN_1 23 // Predefined pin for controlling direction
#define DIRECTION_PIN_2 25
#define STOP_CHAR_RX 101 // Stop chars
#define STOP_CHAR_TX 201
#define LIST_SIZE 3 // RX list size
#define TO_PWM_CONST 2.5 // PWM conversion constant

int8_t buffer[LIST_SIZE]; // String to store incoming data


Ultrasonic distanceSensor0(HC_PIN_0_0, HC_PIN_0_1); 
Ultrasonic distanceSensor1(HC_PIN_1_0, HC_PIN_1_1);
Ultrasonic distanceSensor2(HC_PIN_2_0, HC_PIN_2_1);
Ultrasonic distanceSensor3(HC_PIN_3_0, HC_PIN_3_1);
Ultrasonic distanceSensor4(HC_PIN_4_0, HC_PIN_4_1);

void initMotors()
{
  	digitalWrite(ENABLE_PIN_1, LOW);
  	digitalWrite(ENABLE_PIN_2, LOW);
  	analogWrite(PWM_PIN_1, 0);
  	analogWrite(PWM_PIN_2, 0);
  	digitalWrite(ENABLE_PIN_1, HIGH);
  	digitalWrite(ENABLE_PIN_2, HIGH);
}

void setup()
{
	pinMode(PWM_PIN_1, OUTPUT);
  	pinMode(ENABLE_PIN_1, OUTPUT);
	pinMode(DIRECTION_PIN_1, OUTPUT);
	pinMode(PWM_PIN_2, OUTPUT);
  	pinMode(ENABLE_PIN_2, OUTPUT);
	pinMode(DIRECTION_PIN_2, OUTPUT);
  	initMotors();
	Serial.begin(9600); // Initialize serial communication
}

void writeToMotor(bool left, int8_t inputValue)
{
	// Calculate absolute value of input
	int8_t absValue = abs(inputValue);

	// Determine direction
  	digitalWrite(left ? DIRECTION_PIN_1 : DIRECTION_PIN_2, inputValue < 0 ? HIGH : LOW);


	// Convert absolute char value to PWM value (0 to 100 mapped to 0 to 255)
	absValue = static_cast<int8_t>(floor(absValue*TO_PWM_CONST));

	Serial.println(String(absValue));

	if (absValue<30){
		digitalWrite(left ? ENABLE_PIN_1 : ENABLE_PIN_2, LOW);
		absValue = 30;
	} else {
		digitalWrite(left ? ENABLE_PIN_1 : ENABLE_PIN_2, HIGH);
	}
	

	// Output PWM value
	analogWrite(left ? PWM_PIN_1 : PWM_PIN_2, absValue);
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
	uint8_t sensorValue0 = UINT_DIV_2(distanceSensor0.read(CM)); // divides the number by 2
	uint8_t sensorValue1 = UINT_DIV_2(distanceSensor1.read(CM));
	uint8_t sensorValue2 = UINT_DIV_2(distanceSensor2.read(CM));
	uint8_t sensorValue3 = UINT_DIV_2(distanceSensor3.read(CM));
	uint8_t sensorValue4 = UINT_DIV_2(distanceSensor4.read(CM));
	sensorArray[0] = OOB(sensorValue0); //cuts off the number at 200
	sensorArray[1] = OOB(sensorValue1);
	sensorArray[2] = OOB(sensorValue2);
	sensorArray[3] = OOB(sensorValue3);
	sensorArray[4] = OOB(sensorValue4);
	return sensorArray;
}


void loop()
{
	while(Serial.available()>3){
		Serial.read();
	}
	writeToMotor(true, 50);
	writeToMotor(false, 50);
	delay(1000);
	writeToMotor(true, 0);
	writeToMotor(false, 0);
	delay(1000);
}
