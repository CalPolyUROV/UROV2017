#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);
sensors_event_t event;
int pitch;
int roll;
int yaw;
float accel[3];

void setup() {
  Serial.begin(9600);
  if (!bno.begin()) {  //initialize the sensor
    Serial.print("It's broken, check the wiring");  //its broken
  }
  bno.setExtCrystalUse(true);
}

boolean greater(float value, float threshold) {
  return (value > threshold);
}
boolean below(float value, float threshold) {
  return (value < threshold);
}

void loop() {
  bno.getEvent(&event);
  pitch = event.orientation.y;
  yaw = event.orientation.x;
  roll = event.orientation.z;
  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel[0] = linearaccel[0];
  accel[1] = linearaccel[1];
  accel[2] = linearaccel[2];

    if (greater(accel[0], 1 )) {
      Serial.print("\tForward");
    }
    else if (below(accel[0], -1 )) {
      Serial.print("\tBackward");
    }
    if (greater(accel[1], 1 )) {
      Serial.print("\tLeft");
    }
    else if (below(accel[1], -1 )) {
      Serial.print("\tRight");
    }
    if (greater(accel[2], 1 )) {
      Serial.print("\tUp");
    }
    else if (below(accel[2], -1 )) {
      Serial.print("\tDown");
    }
    Serial.println("");
  
}
