#include <Arduino.h>
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
#define STOP_CHAR_TX 101
#define LIST_SIZE 3 // RX list size
#define TO_PWM_CONST 2.5 // PWM conversion constant
#define MIN_PWM 70

int8_t buffer[LIST_SIZE]; // String to store incoming data


void writeToMotor(bool left, int8_t inputValue)
{
	// Calculate absolute value of input
	uint8_t absValue = abs(inputValue);

	// Determine direction
  	digitalWrite(left ? DIRECTION_PIN_1 : DIRECTION_PIN_2, inputValue < 0 ? HIGH : LOW);


	#ifdef DEBUG
	Serial.println(String(inputValue));
	#endif
	// Convert absolute char value to PWM value (0 to 100 mapped to 0 to 255)
	absValue = static_cast<uint8_t>(floor(absValue*TO_PWM_CONST));

	#ifdef DEBUG
	Serial.println(String(absValue));
	#endif

	if (absValue<MIN_PWM){
		digitalWrite(left ? ENABLE_PIN_1 : ENABLE_PIN_2, LOW);
		absValue = MIN_PWM;
	} else {
		digitalWrite(left ? ENABLE_PIN_1 : ENABLE_PIN_2, HIGH);
	}
	
	#ifdef DEBUG
	Serial.print("l");
	Serial.println(String(absValue));
	#endif
	// Output PWM value
	analogWrite(left ? PWM_PIN_1 : PWM_PIN_2, absValue);
}


void initMotors()
{
  	digitalWrite(ENABLE_PIN_1, LOW);
  	digitalWrite(ENABLE_PIN_2, LOW);
	writeToMotor(true, 0);
	writeToMotor(false, 0);
	digitalWrite(ENABLE_PIN_1, HIGH);
	digitalWrite(ENABLE_PIN_2, HIGH);
	delay(500);
	digitalWrite(ENABLE_PIN_1, LOW);
	digitalWrite(ENABLE_PIN_2, LOW);
}

void setup()
{
	pinMode(PWM_PIN_1, OUTPUT);
  	pinMode(ENABLE_PIN_1, OUTPUT);
	pinMode(DIRECTION_PIN_1, OUTPUT);
	pinMode(PWM_PIN_2, OUTPUT);
  	pinMode(ENABLE_PIN_2, OUTPUT);
	pinMode(DIRECTION_PIN_2, OUTPUT);
	pinMode(HC_PIN_0_0, OUTPUT);
	pinMode(HC_PIN_0_1, INPUT);
	pinMode(HC_PIN_1_0, OUTPUT);
	pinMode(HC_PIN_1_1, INPUT);
	pinMode(HC_PIN_2_0, OUTPUT);
	pinMode(HC_PIN_2_1, INPUT);
	pinMode(HC_PIN_3_0, OUTPUT);
	pinMode(HC_PIN_3_1, INPUT);
	pinMode(HC_PIN_4_0, OUTPUT);
	pinMode(HC_PIN_4_1, INPUT);
  	initMotors();
	Serial.begin(9600); // Initialize serial communication
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
			#ifdef DEBUG
		 	Serial.println("case 0");
			#endif
			writeToMotor(true, buffer[1]);
			writeToMotor(false, buffer[2]);
			break;
		case 1:
			#ifdef DEBUG
			Serial.println("case 1");
			#endif
			writeToMotor(false, buffer[0]);
			writeToMotor(true, buffer[2]);
			break;
		case 2:
			#ifdef DEBUG
			Serial.println("case 2");
			#endif
			writeToMotor(true, buffer[0]);
			writeToMotor(false, buffer[1]);
			break;
		default:
			#ifdef DEBUG
			Serial.println("inv");
			#endif
			break;
		}
	}
	return found;
}

unsigned char readDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Speed of sound is 34 cm/ms

  return static_cast<unsigned char>(distance);
}

void printSensors(){
  Serial.print(char(readDistance(HC_PIN_0_0, HC_PIN_0_1)));
  Serial.print(char(readDistance(HC_PIN_1_0, HC_PIN_1_1)));
  Serial.print(char(readDistance(HC_PIN_2_0, HC_PIN_2_1)));
  Serial.print(char(readDistance(HC_PIN_3_0, HC_PIN_3_1)));
  Serial.print(char(readDistance(HC_PIN_4_0, HC_PIN_4_1)));
  Serial.print(char(STOP_CHAR_TX));
}


void loop()
{
	printSensors();
	/*while(Serial.available()>3){
		Serial.read();
	}
	uint8_t incrementalPointer = 0;
	while(incrementalPointer <= 2)
	{
		if(Serial.available()){
			#ifdef DEBUG
			Serial.println("in available");
			#endif
			char incomingChar = Serial.read();
			buffer[incrementalPointer] = int8_t(incomingChar);
			incrementalPointer++;
		}
	}
	if (incrementalPointer == 3){
		processBuffer();
	}*/
}
