#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//create the instance of the sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//initialize location for event data
sensors_event_t event;

// initialize arrays to store velocity and position in
float pitch;
float roll;
float yaw;
float accel[3];
float velocity[3] = {0, 0, 0};
float position[3] = {0, 0, 0};

//initialize time keeping variables
unsigned long loop_start_time;
float elapsed_time_factor;
int current_time;
int previous_time = 0;

int print_index = 0;
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
  bno.getEvent(&event);

  pitch = event.orientation.x;
  yaw = event.orientation.y;
  roll = event.orientation.z;

  //get the accelerometer vector as raw data
  imu::Vector<3> linearacc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel[0] = filter_accel(linearacc[0]) * cos(orientation[0] * PI / 180) + sin(orientation[0] * PI / 180);
  accel[1] = filter_accel(linearacc[1]) * cos(orientation[0] * PI / 180) + sin(orientation[0] * PI / 180);
  accel[2] = filter_accel(linearacc[2]) * cos(orientation[2] * PI / 180);

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

  if (print_index == 20) {


    //display the data
    Serial.print("\tPitch: ");
    Serial.print(event.orientation.y, 4);
    Serial.print("\tYaw: ");
    Serial.print(event.orientation.x, 4);
    Serial.print("\tRoll: ");
    Serial.print(event.orientation.z, 4);

    serial_print(accel, "Accel");
    // serial_print(velocity, "Velocity");
    // serial_print(position, "Position");
 delay(100);
    print_index = 0;
  }
  previous_time = current_time;
  //delay(50);

  //next line
  Serial.println("");
  print_index++;
}
