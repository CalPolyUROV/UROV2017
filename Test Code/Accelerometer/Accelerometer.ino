#include "Accel.h"

Accel accelerometer;

//initialize time keeping variables
unsigned long loop_start_time;
//float elapsed_time_factor;
int current_time;
int previous_time = 0;

int print_index = 0;

void setup() {
  accelerometer = Accel();
  Serial.begin(9600);
  Serial.println("Testing Orientation and Acceleration Sensor");
  Serial.println("");
  //open serial
}

void serial_print(float data_array[], String name) {

  Serial.print("\t" + name + " X: ");
  Serial.print(data_array[0], 2);
  Serial.print("\t" + name + " Y: ");
  Serial.print(data_array[1], 2);
  Serial.print("\t" + name + " z: ");
  Serial.print(data_array[2], 2);
}

//float lookup_sin(float input)
//{
//  return sin_list[(int)input];
//}
//
//float lookup_cos(float input)
//{
//  return sin_list[phaseshift((int) input)];
//}

//int phaseshift(int value)
//{
//  int out = value + 90;
//  if (out > 360)
//  {
//    out = - 360;
//  }
//  return out;
//}

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
  //  elapsed_time_factor = ( current_time - previous_time) / 1000.0;
  //will glitch out every five days due to overflow, this will break everything
  //andrew's fix was to check if the later was greater

  //  position[0] = position[0] + (velocity[0] * elapsed_time_factor);
  //  position[1] = position[1] + (velocity[1] * elapsed_time_factor);
  //  position[2] = position[2] + (velocity[2] * elapsed_time_factor);
  //
  //  velocity[0] = velocity[0] + (accel[0] * elapsed_time_factor);
  //  velocity[1] = velocity[1] + (accel[1] * elapsed_time_factor);
  //  velocity[2] = velocity[2] + (accel[2] * elapsed_time_factor);

  if (print_index == 20) {

    Serial.print("\t Elapsed time: ");
    Serial.print(current_time - previous_time);
    previous_time = current_time;

    //display the data
    Serial.print("\tPitch: ");
    Serial.print(accelerometer.getPitch());
    Serial.print("\tYaw: ");
    Serial.print(accelerometer.getYaw());
    Serial.print("\tRoll: ");
    Serial.print(accelerometer.getRoll());

    //    if (threshold(accel[0]) || threshold(accel[1]) || threshold(accel[2]))
    //    {
    //      serial_print(accel, "Accel");
    //    }

    if (greater(accelerometer.getAccelX(), 1 ))
    {
      Serial.print("\tForward");
    }
    else if (below(accelerometer.getAccelX(), -1 ))
    {
      Serial.print("\tBackward");
    }
    if (greater(accelerometer.getAccelY(), 1 ))
    {
      Serial.print("\tLeft");
    }
    else if (below(accelerometer.getAccelY(), -1 ))
    {
      Serial.print("\Right");
    }
    if (greater(accelerometer.getAccelZ(), 1 ))
    {
      Serial.print("\tUp");
    }
    else if (below(accelerometer.getAccelZ(), -1 ))
    {
      Serial.print("\tDown");
    }


    print_index = 0;

    //next line
    Serial.println("");
  }

  print_index++;
}
