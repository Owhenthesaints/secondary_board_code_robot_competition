#define NUM_SENSORS 5 // Utilisation de 4 capteurs à ultrasons pour éviter les broches 20 et 21
#include <Arduino.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX_VAL 100
#define STOP_CHAR_DIST 101

const int triggerPins[NUM_SENSORS] = {30, 32, 34, 36, 38}; // Broches pour les triggers
const int echoPins[NUM_SENSORS] = {2, 3, 18, 19, 20};	  // Broches pour les echos, compatibles avec les interruptions
uint8_t distances[NUM_SENSORS] = {0, 0, 0, 0, 0};

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
	Serial.begin(9600);

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
	while(Serial.available()){
		Serial.read();
	}
	getAndSendDistances();
}
