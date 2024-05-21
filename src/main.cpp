#define NUM_SENSORS 5 // Utilisation de 4 capteurs à ultrasons pour éviter les broches 20 et 21
#include <Arduino.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX_VAL 100
#define STOP_CHAR_DIST 101
#define PWM_PIN_1 4 // Predefined PWM output pin
#define PWM_PIN_2 5
#define ENABLE_PIN_1 22
#define ENABLE_PIN_2 24
#define DIRECTION_PIN_1 23 // Predefined pin for controlling direction
#define DIRECTION_PIN_2 25
#define STOP_CHAR_RX 101 // Stop chars
#define STOP_CHAR_TX 101
#define LIST_SIZE 3 // RX list size
#define TO_PWM_CONST 2.5 // PWM conversion constant
#define MIN_PWM 70
#define NUM_DIST_SENSORS 5
#define STOP_CHAR 101
#define MAX_TIME 1000

const int triggerPins[NUM_SENSORS] = {30, 32, 34, 36, 38}; // Broches pour les triggers
const int echoPins[NUM_SENSORS] = {2, 3, 18, 19, 20};	  // Broches pour les echos, compatibles avec les interruptions
uint8_t distances[NUM_SENSORS] = {0, 0, 0, 0, 0};
int8_t buffer[LIST_SIZE]; // String to store incoming data

inline bool TimeExceeded(long int & intervalTime){
	return static_cast<long int>(intervalTime + MAX_TIME - millis()) < 0;
}

volatile unsigned long startTimes[NUM_SENSORS];
volatile unsigned long echoTimes[NUM_SENSORS];
volatile bool newMeasurementAvailable[NUM_SENSORS];

void ISREcho(uint8_t i)
{
	if (digitalRead(echoPins[i]) == HIGH)
	{
		startTimes[i] = micros();
	}
	else
	{
		echoTimes[i] = micros();
		newMeasurementAvailable[i] = true;
	}
}

void ISREcho1() {
	ISREcho(0);
}

void ISREcho2() {
	ISREcho(1);
}


void ISREcho3() {
	ISREcho(2);
}


void ISREcho4() {
	ISREcho(3);
}

void ISREcho5() {
	ISREcho(4);
}

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

inline void stopMotors()
{
	writeToMotor(false, 0);
	writeToMotor(true, 0);
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

void motorSetup() {
	pinMode(PWM_PIN_1, OUTPUT);
  	pinMode(ENABLE_PIN_1, OUTPUT);
	pinMode(DIRECTION_PIN_1, OUTPUT);
	pinMode(PWM_PIN_2, OUTPUT);
  	pinMode(ENABLE_PIN_2, OUTPUT);
	pinMode(DIRECTION_PIN_2, OUTPUT);
}


/**
 * @brief Processes the buffer tries to find position of the stop char and write two values to motor
 * @returns bool indicating if the stop char has been found
*/
bool processBuffer()
{
	// Extract last two characters separated by STOP_CHAR
	// Extract last two characters
	uint8_t index;
	bool found = false;
	// find stop char
	for (uint8_t i =0; i<LIST_SIZE; i++){
		if (buffer[i]==STOP_CHAR_RX){
			index = i;
			found = true;
			continue;
		}
	}

	if (found)
	{
		// handling the different cases of position of stop_char
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


void getAndSendDistances() {
	for (int i = 0; i < NUM_SENSORS; i++)
	{
		if (!newMeasurementAvailable[i])
		{
			digitalWrite(triggerPins[i], LOW);
			delayMicroseconds(2);
			digitalWrite(triggerPins[i], HIGH);
			startTimes[i] = micros();
			delayMicroseconds(10);
			digitalWrite(triggerPins[i], LOW);

			// Ajouter un délai pour éviter les interférences entre les capteurs
			delay(5);
		}
	}

	for (uint8_t i(0); i < NUM_SENSORS; i++)
	{
		if (newMeasurementAvailable[i])
		{
			noInterrupts(); // Désactiver les interruptions pour éviter des lectures incorrectes
			unsigned long duration = echoTimes[i] - startTimes[i];
			interrupts(); // Réactiver les interruptions

			float distance = duration * 0.034 / 2;
			distances[i] = static_cast<uint8_t>(MAX(MIN(distance, MAX_VAL), 0));
			newMeasurementAvailable[i] = false;
		}
	}

	delay(10);
	for (uint8_t i(0); i<NUM_SENSORS; i++){
		Serial.print(char(distances[i]));
	}
	Serial.print(char(STOP_CHAR_DIST));

}

void setup()
{
	motorSetup();
  	initMotors();
	Serial.begin(9600); // Initialize serial communication

	for (int i = 0; i < NUM_SENSORS; i++)
	{
		pinMode(triggerPins[i], OUTPUT);
		pinMode(echoPins[i], INPUT);
		newMeasurementAvailable[i] = false;
	}
	
	attachInterrupt(digitalPinToInterrupt(echoPins[0]), ISREcho1, CHANGE);
	attachInterrupt(digitalPinToInterrupt(echoPins[1]), ISREcho2, CHANGE);
	attachInterrupt(digitalPinToInterrupt(echoPins[2]), ISREcho3, CHANGE);
	attachInterrupt(digitalPinToInterrupt(echoPins[3]), ISREcho4, CHANGE);
	attachInterrupt(digitalPinToInterrupt(echoPins[4]), ISREcho5, CHANGE);
}

void loop()
{
	static long int intervalTime(millis());
	static bool firstTime(true);
	while (Serial.available() > 3)
	{
		Serial.read();

	}
	intervalTime = millis();
	uint8_t incremental_pointer(0);
	while (incremental_pointer <= 2)
	{
		//if (TimeExceeded(intervalTime))
		//{
		//	stopMotors();
		//	break;
		//}
		//else if (Serial.available())
		//{
		//	char incommingChar = Serial.read();
		//	buffer[incremental_pointer] = int8_t(incommingChar);
		//	incremental_pointer++;
		//	firstTime = false;
		//}
		if (Serial.available())
		{
			char incommingChar = Serial.read();
			buffer[incremental_pointer] = int8_t(incommingChar);
			incremental_pointer ++;
		}
	}
	if(incremental_pointer == 3){
		processBuffer();
	}
	getAndSendDistances();
}
