#include "Arduino.h"
#include "Accel.h"

//initialize time keeping variables
unsigned long loop_start_time;
int current_time;
int previous_time = 0;

Accel *accelerometer;

void setup() {
  Serial.begin(9600);
  *accelerometer = Accel();
  Serial.println(accelerometer->init());
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

  Serial.print(accelerometer->refresh());

  Serial.print("\t Elapsed time: ");
  Serial.print(current_time - previous_time);
  previous_time = current_time;

  //display the data
  Serial.print("\tPitch: ");
  Serial.print(accelerometer->getPitch());
  Serial.print("\tYaw: ");
  Serial.print(accelerometer->getYaw());
  Serial.print("\tRoll: ");
  Serial.print(accelerometer->getRoll());

  Serial.print("\accelX: ");
  Serial.print(accelerometer->getAccelX());
  Serial.print("\tYaw: ");
  Serial.print(accelerometer->getAccelY());
  Serial.print("\tRoll: ");
  Serial.print(accelerometer->getAccelZ());

  if (greater(accelerometer->getAccelX(), 1 ))
  {
    Serial.print("\tForward");
  }
  else if (below(accelerometer->getAccelX(), -1 ))
  {
    Serial.print("\tBackward");
  }
  if (greater(accelerometer->getAccelY(), 1 ))
  {
    Serial.print("\tLeft");
  }
  else if (below(accelerometer->getAccelY(), -1 ))
  {
    Serial.print("\Right");
  }
  if (greater(accelerometer->getAccelZ(), 1 ))
  {
    Serial.print("\tUp");
  }
  else if (below(accelerometer->getAccelZ(), -1 ))
  {
    Serial.print("\tDown");
  }

  //next line
  Serial.println("");

}
