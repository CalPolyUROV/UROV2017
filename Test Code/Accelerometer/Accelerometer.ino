#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//create the instance of the sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// initialize arrays to store velocity and position in
float orientation[3];
float accel[3];
float velocity[3] = {0, 0, 0};
float position[3] = {0, 0, 0};

//initialize time keeping variables
unsigned long loop_start_time;
float elapsed_time_factor;

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
  delay(50);

  //use for better low power timing?
  bno.setExtCrystalUse(true);
}

void serial_print(float data_array[], String name) {
  //display the data
  Serial.print("\t" + name + " X: ");
  Serial.print(data_array[0], 2);
  Serial.print("\t" + name + " Y: ");
  Serial.print(data_array[1], 2);
  Serial.print("\t" + name + " z: ");
  Serial.print(data_array[2], 2);
}

void loop() {

  loop_start_time = millis();

  //get the orrientation sensor event
  sensors_event_t event;
  bno.getEvent(&event);

  orientation[0] = event.orientation.x;
  orientation[1] = event.orientation.y;
  orientation[2] = event.orientation.z;

  //display the data
  //  Serial.print("Yaw: ");
  //  Serial.print(orientation[0], 4);
  //  Serial.print("\tPitch: ");
  //  Serial.print(orientation[1], 4);
  //  Serial.print("\tRoll: ");
  //  Serial.print(orientation[2], 4);

  //get the accelerometer vector as raw data
  imu::Vector<3> linearacc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel[0] = linearacc[0];
  accel[1] = linearacc[1];
  accel[2] = linearacc[2];

  Serial.print("\tAccel X: ");
  Serial.print(accel[0], 4);
  Serial.print("\tAccel Y: ");
  Serial.print(accel[1], 4);
  Serial.print("\tAccel Z: ");
  Serial.print(accel[2], 4);

  serial_print(accel, "Accel");

  delay(50);
  elapsed_time_factor = (millis() - loop_start_time) / 1000;
  //will glitch out every five days due to overflow, this will break everything

  velocity[0] = velocity[0] + (accel[0] * elapsed_time_factor);
  velocity[1] = velocity[1] + (accel[1] * elapsed_time_factor);
  velocity[2] = velocity[2] + (accel[2] * elapsed_time_factor);
  position[0] = position[0] + (velocity[0] * elapsed_time_factor);
  position[1] = position[1] + (velocity[1] * elapsed_time_factor);
  position[2] = position[2] + (velocity[2] * elapsed_time_factor);

  //next line
  Serial.println("");

}
