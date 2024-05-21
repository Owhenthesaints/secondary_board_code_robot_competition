#include <Arduino.h>
const int numSensors = 5;

const int triggerPins[numSensors] = {30, 32, 34, 36, 38}; // Broches pour les triggers, modifiables
const int echoPins[numSensors] = {2, 3, 18, 19, 20}; // Broches pour les echos, doivent être compatibles avec les interruptions

volatile long startTimes[numSensors];
volatile long echoTimes[numSensors];
volatile bool newMeasurementAvailable[numSensors];

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < numSensors; i++) {
    pinMode(triggerPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    attachInterrupt(digitalPinToInterrupt(echoPins[i]), ISR_echo, CHANGE);
    newMeasurementAvailable[i] = false;
  }
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    if (!newMeasurementAvailable[i]) {
      digitalWrite(triggerPins[i], LOW);
      delayMicroseconds(2);
      digitalWrite(triggerPins[i], HIGH);
      delayMicroseconds(10);
      digitalWrite(triggerPins[i], LOW);
    }
  }

  for (int i = 0; i < 2; i++) {
    if (newMeasurementAvailable[i]) {
      long duration = echoTimes[i] - startTimes[i];
      float distance = duration * 0.034 / 2;
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(distance);
      newMeasurementAvailable[i] = false;
    }
  }

  delay(100);
}

void ISR_echo() {
  for (int i = 0; i < numSensors; i++) {
    if (digitalRead(echoPins[i]) == HIGH) {
      startTimes[i] = micros();
    } else {
      echoTimes[i] = micros();
      newMeasurementAvailable[i] = true;
    }
  }
}

