#ifndef Accel_h
#define Accel_h

//#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

class Accel
{
    Adafruit_BNO055* bno;
    //sensors_event_t event;
    int pitch;
    int roll;
    int yaw;
    float accelVector[3];
  public:
    Accel();
    String init();
    String refresh();
    int getPitch();
    int getYaw();
    int getRoll();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
};

#endif
