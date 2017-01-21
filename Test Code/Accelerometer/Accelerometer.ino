#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//create the instance of the sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//initialize location for event data
sensors_event_t event;

// initialize arrays to store velocity and position in
int pitch;
int roll;
int yaw;
float accel[3];
float velocity[3] = {0, 0, 0};
float position[3] = {0, 0, 0};

//initialize time keeping variables
unsigned long loop_start_time;
float elapsed_time_factor;
int current_time;
int previous_time = 0;

int sin_list[] = {0.00, 0.02, 0.03, 0.05, 0.07, 0.09, 0.10, 0.12, 0.14, 0.16, 0.17, 0.19, 0.21, 0.22, 0.24, 0.26, 0.28, 0.29, 0.31, 0.33, 0.34, 0.36, 0.37, 0.39, 0.41, 0.42, 0.44, 0.45, 0.47, 0.48, 0.50, 0.52, 0.53, 0.54, 0.56, 0.57, 0.59, 0.60, 0.62, 0.63, 0.64, 0.66, 0.67, 0.68, 0.69, 0.71, 0.72, 0.73, 0.74, 0.75, 0.77, 0.78, 0.79, 0.80, 0.81, 0.82, 0.83, 0.84, 0.85, 0.86, 0.87, 0.87, 0.88, 0.89, 0.90, 0.91, 0.91, 0.92, 0.93, 0.93, 0.94, 0.95, 0.95, 0.96, 0.96, 0.97, 0.97, 0.97, 0.98, 0.98, 0.98, 0.99, 0.99, 0.99, 0.99, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 0.99, 0.99, 0.99, 0.99, 0.98, 0.98, 0.98, 0.97, 0.97, 0.97, 0.96, 0.96, 0.95, 0.95, 0.94, 0.93, 0.93, 0.92, 0.91, 0.91, 0.90, 0.89, 0.88, 0.87, 0.87, 0.86, 0.85, 0.84, 0.83, 0.82, 0.81, 0.80, 0.79, 0.78, 0.77, 0.75, 0.74, 0.73, 0.72, 0.71, 0.69, 0.68, 0.67, 0.66, 0.64, 0.63, 0.62, 0.60, 0.59, 0.57, 0.56, 0.54, 0.53, 0.52, 0.50, 0.48, 0.47, 0.45, 0.44, 0.42, 0.41, 0.39, 0.37, 0.36, 0.34, 0.33, 0.31, 0.29, 0.28, 0.26, 0.24, 0.22, 0.21, 0.19, 0.17, 0.16, 0.14, 0.12, 0.10, 0.09, 0.07, 0.05, 0.03, 0.02, -0.00, -0.02, -0.03, -0.05, -0.07, -0.09, -0.10, -0.12, -0.14, -0.16, -0.17, -0.19, -0.21, -0.22, -0.24, -0.26, -0.28, -0.29, -0.31, -0.33, -0.34, -0.36, -0.37, -0.39, -0.41, -0.42, -0.44, -0.45, -0.47, -0.48, -0.50, -0.52, -0.53, -0.54, -0.56, -0.57, -0.59, -0.60, -0.62, -0.63, -0.64, -0.66, -0.67, -0.68, -0.69, -0.71, -0.72, -0.73, -0.74, -0.75, -0.77, -0.78, -0.79, -0.80, -0.81, -0.82, -0.83, -0.84, -0.85, -0.86, -0.87, -0.87, -0.88, -0.89, -0.90, -0.91, -0.91, -0.92, -0.93, -0.93, -0.94, -0.95, -0.95, -0.96, -0.96, -0.97, -0.97, -0.97, -0.98, -0.98, -0.98, -0.99, -0.99, -0.99, -0.99, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -1.00, -0.99, -0.99, -0.99, -0.99, -0.98, -0.98, -0.98, -0.97, -0.97, -0.97, -0.96, -0.96, -0.95, -0.95, -0.94, -0.93, -0.93, -0.92, -0.91, -0.91, -0.90, -0.89, -0.88, -0.87, -0.87, -0.86, -0.85, -0.84, -0.83, -0.82, -0.81, -0.80, -0.79, -0.78, -0.77, -0.75, -0.74, -0.73, -0.72, -0.71, -0.69, -0.68, -0.67, -0.66, -0.64, -0.63, -0.62, -0.60, -0.59, -0.57, -0.56, -0.54, -0.53, -0.52, -0.50, -0.48, -0.47, -0.45, -0.44, -0.42, -0.41, -0.39, -0.37, -0.36, -0.34, -0.33, -0.31, -0.29, -0.28, -0.26, -0.24, -0.22, -0.21, -0.19, -0.17, -0.16, -0.14, -0.12, -0.10, -0.09, -0.07, -0.05, -0.03, -0.02};

int print_index = 0;

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

  //use for better low power timing?
  bno.setExtCrystalUse(true);
}

void serial_print(float data_array[], String name) {

  Serial.print("\t" + name + " X: ");
  Serial.print(data_array[0], 2);
  Serial.print("\t" + name + " Y: ");
  Serial.print(data_array[1], 2);
  Serial.print("\t" + name + " z: ");
  Serial.print(data_array[2], 2);
}

float lookup_sin(float input)
{
  return sin_list[(int)input];
}

float lookup_cos(float input)
{
  return sin_list[phaseshift((int) input)];
}

int phaseshift(int value)
{
  int out = value + 90;
  if (out > 360)
  {
    out = - 360;
  }
  return out;
}

void loop() {

  //get the orrientation sensor event
  bno.getEvent(&event);

  pitch = event.orientation.y;
  yaw = event.orientation.x;
  roll = event.orientation.z;

  //get the accelerometer vector as raw data
  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel[0] = (linearaccel[0] * lookup_cos(yaw) * lookup_cos(pitch)) + (linearaccel[1] * lookup_sin(yaw)) + (linearaccel[2] * lookup_sin(pitch));
  accel[1] = (linearaccel[1] * lookup_cos(yaw) * lookup_cos(roll)) + (linearaccel[0] * lookup_sin(yaw)) + (linearaccel[2] * lookup_sin(roll));
  accel[2] = (linearaccel[2] * lookup_cos(roll) * lookup_cos(pitch)) + (linearaccel[1] * lookup_sin(roll)) + (linearaccel[0] * lookup_sin(pitch));

  current_time = millis();
  elapsed_time_factor = ( current_time - previous_time) / 1000.0;
  //will glitch out every five days due to overflow, this will break everything
  //andrew's fix was to check if the later was greater

  position[0] = position[0] + (velocity[0] * elapsed_time_factor);
  position[1] = position[1] + (velocity[1] * elapsed_time_factor);
  position[2] = position[2] + (velocity[2] * elapsed_time_factor);

  velocity[0] = velocity[0] + (accel[0] * elapsed_time_factor);
  velocity[1] = velocity[1] + (accel[1] * elapsed_time_factor);
  velocity[2] = velocity[2] + (accel[2] * elapsed_time_factor);

  //  Serial.print("\t Elapsed time: ");
  // Serial.print(current_time - previous_time);

  if (print_index == 20) {
    //display the data
    Serial.print("\tPitch: ");
    Serial.print(pitch);
    Serial.print("\tYaw: ");
    Serial.print(yaw);
    Serial.print("\tRoll: ");
    Serial.print(event.orientation.z, 4);

    serial_print(accel, "Accel");
    // serial_print(velocity, "Velocity");
    // serial_print(position, "Position");
    delay(100);
    print_index = 0;

    //next line
    Serial.println("");
  }
  previous_time = current_time;

  //next line
//  Serial.println("");

  print_index++;
}
