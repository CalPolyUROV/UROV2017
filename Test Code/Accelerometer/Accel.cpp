#include "Arduino.h"
//#include <utility/imumaths.h>
#include "Accel.h"

Accel::Accel()
{
  //  Serial.begin(9600);
  Serial.println("Testing Orientation and Acceleration Sensor");
  //  //initialize the sensor
  *bno = Adafruit_BNO055(55);
  Serial.println("Initialized sensor");
  if (!bno->begin())
  {
    //its broken
    Serial.println("It's broken, check the wiring");
  }
  //use for better low power timing?
  // bno->setExtCrystalUse(true);

  pitch = 0;
  roll = 0;
  yaw = 0;
}

String Accel::init() {
  return ("Tested Orientation and Acceleration Sensor");
}

String Accel::refresh() {
  //get the orrientation sensor event
//  bno->getEvent(&event);
//  pitch = event.orientation.y;
//  yaw = event.orientation.x;
//  roll = event.orientation.z;

  imu::Vector<3> oVector = bno->getVector(Adafruit_BNO055::VECTOR_EULER);
  pitch = (int)oVector.y();
  yaw = (int)oVector.x();
  roll = (int)oVector.z();

  //get the accelerometer vector as raw data
  imu::Vector<3> linearaccel = bno->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accelVector[0] = linearaccel[0];
  accelVector[1] = linearaccel[1];
  accelVector[2] = linearaccel[2];

  Serial.print(pitch);
  Serial.print(yaw);
  Serial.print(roll);

  return (" updated");
}
int Accel::getPitch()
{
  //return 123;
  return pitch;
}
int Accel::getYaw()
{
  // return 234;
  return yaw;
}
int Accel::getRoll()
{
  //return 345;
  return roll;
}

float Accel::getAccelX()
{
  // return 1.234;
  return accelVector[0];
}
float Accel::getAccelY()
{
  //return 2.345;
  return accelVector[1];
}
float Accel::getAccelZ()
{
  //return 3.456;
  return accelVector[2];
}

