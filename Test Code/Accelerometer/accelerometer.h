#ifdef Accelerometer_h
#define Acccelerometer.h

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

class Accelerometer
{
  public:
    Accelerometer();
    void update();
    int getPitch();
    int getYaw();
    int getRoll();
  protected:
    Adafruit_BNO055 bno;
    sensors_event_t event;
    int pitch;
    int roll;
    int yaw;
    float accel[3];
}

#endif
