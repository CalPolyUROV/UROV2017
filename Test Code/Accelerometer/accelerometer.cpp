#include "Arduino.h"
//#include <utility/imumaths.h>
#include "Accelerometer.h"

Accelerometer::Accelerometer()
{
  bno = Adafruit_BNO055(55)

  //initialize the sensor
  if (!bno.begin())
  {
    //its broken
    Serial.print("It's broken, check the wiring");
  }

  //use for better low power timing?
  bno.setExtCrystalUse(true);
}

void Accelerometer::update() {
  //get the orrientation sensor event
  bno.getEvent(&event);

  pitch = event.orientation.y;
  yaw = event.orientation.x;
  roll = event.orientation.z;

  //get the accelerometer vector as raw data
  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel[0] = linearaccel[0];
  accel[1] = linearaccel[1];
  accel[2] = linearaccel[2];

  // accel[0] = (linearaccel[0] * lookup_cos(yaw) * lookup_cos(pitch)) + (linearaccel[1] * lookup_sin(yaw)) + (linearaccel[2] * lookup_sin(pitch));
  // accel[1] = (linearaccel[1] * lookup_cos(yaw) * lookup_cos(roll)) + (linearaccel[0] * lookup_sin(yaw)) + (linearaccel[2] * lookup_sin(roll));
  // accel[2] = (linearaccel[2] * lookup_cos(roll) * lookup_cos(pitch)) + (linearaccel[1] * lookup_sin(roll)) + (linearaccel[0] * lookup_sin(pitch));

}
int Accelerometer::getPitch()
{
  return pitch;
}
int Accelerometer::getYaw()
{
  return yaw;
}
int Accelerometer::getRoll()
{
  return roll;
}
int[3] Accelerometer::getAccel()
{
return accel;
}

