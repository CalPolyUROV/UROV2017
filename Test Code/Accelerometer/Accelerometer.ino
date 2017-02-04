#include "Arduino.h"
#include "Accel.h"

//initialize time keeping variables
unsigned long loop_start_time;
int current_time;
int previous_time = 0;

Accel* accelerometer;

void setup() {
  Serial.begin(9600);
  Serial.println("Serial Works");
  *accelerometer = Accel();
 // Serial.println(accelerometer->init());
 Serial.println("Printing works");
 Serial.flush();
}

void serial_print(float data_array[], String name) {

  Serial.print("\t" + name + " X: ");
  Serial.print(data_array[0], 2);
  Serial.print("\t" + name + " Y: ");
  Serial.print(data_array[1], 2);
  Serial.print("\t" + name + " z: ");
  Serial.print(data_array[2], 2);
}

boolean greater(float value, float threshold)
{
  return (value > threshold);
}
boolean below(float value, float threshold)
{
  return (value < threshold);
}

void loop() {
  current_time = millis();

 accelerometer->update();

  //  position[0] = position[0] + (velocity[0] * elapsed_time_factor);
  //  position[1] = position[1] + (velocity[1] * elapsed_time_factor);
  //  position[2] = position[2] + (velocity[2] * elapsed_time_factor);
  //
  //  velocity[0] = velocity[0] + (accel[0] * elapsed_time_factor);
  //  velocity[1] = velocity[1] + (accel[1] * elapsed_time_factor);
  //  velocity[2] = velocity[2] + (accel[2] * elapsed_time_factor);

  Serial.print("\t Elapsed time: ");
  Serial.print(current_time - previous_time);
  previous_time = current_time;

  //display the data
//  Serial.print("\tPitch: ");
//  Serial.print(accelerometer.getPitch());
//  Serial.print("\tYaw: ");
//  Serial.print(accelerometer.getYaw());
//  Serial.print("\tRoll: ");
//  Serial.print(accelerometer.getRoll());
//
//  if (greater(accelerometer.getAccelX(), 1 ))
//  {
//    Serial.print("\tForward");
//  }
//  else if (below(accelerometer.getAccelX(), -1 ))
//  {
//    Serial.print("\tBackward");
//  }
//  if (greater(accelerometer.getAccelY(), 1 ))
//  {
//    Serial.print("\tLeft");
//  }
//  else if (below(accelerometer.getAccelY(), -1 ))
//  {
//    Serial.print("\Right");
//  }
//  if (greater(accelerometer.getAccelZ(), 1 ))
//  {
//    Serial.print("\tUp");
//  }
//  else if (below(accelerometer.getAccelZ(), -1 ))
//  {
//    Serial.print("\tDown");
//  }
//
//  //next line
//  Serial.println("");

}
