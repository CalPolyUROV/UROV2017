#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup() {
  Serial.begin(9600);
  Serial.println("Testing Orientation Sensor");
  Serial.println("");

  //initialize the sensor
  if (!bno.begin())
  {
    //its broken
    Serial.print("It's broken, check the wiring");
  }
  delay(1000);

  bno.setExtCrystalUse(true);
}

void loop() {

  //get the event
  sensors_event_t event;
  bno.getEvent(&event);

  //display the data
  Serial.print("Yaw: ");
  Serial.print(event.orientation.x, 4);
  Serial.print("\tPitch: ");
  Serial.print(event.orientation.y, 4);
  Serial.print("\tRoll: ");
  Serial.print(event.orientation.z, 4);
  Serial.println("");

}
