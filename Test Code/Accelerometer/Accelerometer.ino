#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//create the instance of the sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//initialize time keeping variables
unsigned long loop_start_time;
unsigned long elapsed_time_factor;

void setup() {
  Serial.begin(9600);
  Serial.println("Testing Orientation and Acceleration Sensor");
  Serial.println("");
  //open serial

  //initialize the sensor
  if (!bno.begin())
  {
    //its broken
    Serial.print("It's broken, check the wiring");
  }
  delay(1000);

  //use for better low power timing?
  bno.setExtCrystalUse(true);
}

void loop() {

  loop_start_time = millis();

  //get the orrientation sensor event
  sensors_event_t event;
  bno.getEvent(&event);

  //display the data
  Serial.print("Yaw: ");
  Serial.print(event.orientation.x, 4);
  Serial.print("\tPitch: ");
  Serial.print(event.orientation.y, 4);
  Serial.print("\tRoll: ");
  Serial.print(event.orientation.z, 4);

  //get the accelerometer vector as raw data
  imu::Vector<3> linearacc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  Serial.print("\tAccel X: ");
  Serial.print(linearacc[0], 4);
  Serial.print("\tAccel Y: ");
  Serial.print(linearacc[1], 4);
  Serial.print("\tAccel Z: ");
  Serial.print(linearacc[2], 4);
  delay(50);
  elapsed_time_factor = (millis() - loop_start_time) / 1000;
  //will glitch out every five days due to overflow, this will break everything

  //next line
  Serial.println("");

}
