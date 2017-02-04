#include "Arduino.h"
//#include <utility/imumaths.h>
#include "Accel.h"

Accel::Accel()
{
  //  Serial.begin(9600);
  Serial.println("Testing Orientation and Acceleration Sensor");
  //  Serial.flush();
  //  //initialize the sensor
  *bno = Adafruit_BNO055(55);
  Serial.println("initialized sensor");
  if (!bno->begin(AMG))
  {
    //its broken
    Serial.println("It's broken, check the wiring");
  }
  //use for better low power timing?
 // bno->setExtCrystalUse(true);
  Serial.println("Finished constructor");
  Serial.flush();
}

String Accel::init() {
  return ("Testing Orientation and Acceleration Sensor");
}

void Accel::update() {
  //get the orrientation sensor event
  bno->getEvent(event);

  pitch = event->orientation.y;
  yaw = event->orientation.x;
  roll = event->orientation.z;

  //get the accelerometer vector as raw data
  imu::Vector<3> linearaccel = bno->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accelVector[0] = linearaccel[0];
  accelVector[1] = linearaccel[1];
  accelVector[2] = linearaccel[2];

  // accel[0] = (linearaccel[0] * lookup_cos(yaw) * lookup_cos(pitch)) + (linearaccel[1] * lookup_sin(yaw)) + (linearaccel[2] * lookup_sin(pitch));
  // accel[1] = (linearaccel[1] * lookup_cos(yaw) * lookup_cos(roll)) + (linearaccel[0] * lookup_sin(yaw)) + (linearaccel[2] * lookup_sin(roll));
  // accel[2] = (linearaccel[2] * lookup_cos(roll) * lookup_cos(pitch)) + (linearaccel[1] * lookup_sin(roll)) + (linearaccel[0] * lookup_sin(pitch));

}
int Accel::getPitch()
{
  return pitch;
}
int Accel::getYaw()
{
  return yaw;
}
int Accel::getRoll()
{
  return roll;
}

float Accel::getAccelX()
{
  return accelVector[0];
}
float Accel::getAccelY()
{
  return accelVector[1];
}
float Accel::getAccelZ()
{
  return accelVector[2];
}

