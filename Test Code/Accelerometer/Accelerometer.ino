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
int current_time;
int previous_time = 0;

float filter_value = 0.2;

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

float filter_accel(float value) {
  if (value > (-1 * filter_value) && value < filter_value) {
    return 0;
  }
  return value;
}

void loop() {

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
  accel[0] = filter_accel(linearacc[0]);
  accel[1] = filter_accel(linearacc[1]);
  accel[2] = filter_accel(linearacc[2]);

  serial_print(accel, "Accel");

  current_time = millis();
  elapsed_time_factor = ( current_time - previous_time) / 1000.0;
  //will glitch out every five days due to overflow, this will break everything
  //andrew's fix was to check if the later was greater



  Serial.print("\t Elapsed time: ");
  Serial.print(current_time - previous_time);

  position[0] = position[0] + (velocity[0] * elapsed_time_factor);
  position[1] = position[1] + (velocity[1] * elapsed_time_factor);
  position[2] = position[2] + (velocity[2] * elapsed_time_factor);

  velocity[0] = velocity[0] + (accel[0] * elapsed_time_factor);
  velocity[1] = velocity[1] + (accel[1] * elapsed_time_factor);
  velocity[2] = velocity[2] + (accel[2] * elapsed_time_factor);


  serial_print(velocity, "Velocity");
  //serial_print(position, "Position");

  previous_time = current_time;
  delay(50);

  //next line
  Serial.println("");

}
