#include "Arduino.h"
#include "Accel.h"

Accel *accelerometer;

void setup() {
  Serial.begin(9600);
  Serial.println("Serial Works");
  *accelerometer = Accel();
  Serial.println(accelerometer->init());
  Serial.println("Printing works");

}

void loop() {
  Serial.println("starting loop");

  Serial.println(accelerometer->refresh());

  Serial.println(accelerometer->getPitch());
  Serial.println(accelerometer->getRoll());
  Serial.println(accelerometer->getYaw());

  Serial.println(accelerometer->getAccelX());
  Serial.println(accelerometer->getAccelY());
  Serial.println(accelerometer->getAccelZ());
  delay(500);
}
