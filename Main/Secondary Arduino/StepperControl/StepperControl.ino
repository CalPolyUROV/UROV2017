// MultiStepper
// -*- mode: C++ -*-
//
// Control both Stepper motors at the same time with different speeds
// and accelerations. 
// Requires the AFMotor library (https://github.com/adafruit/Adafruit-Motor-Shield-library)
// And AccelStepper with AFMotor support (https://github.com/adafruit/AccelStepper)
// Public domain!

#include <AccelStepper.h>
#include <AFMotor.h>

// two stepper motors one on each port
AF_Stepper motor1(200, 1);
AF_Stepper motor2(200, 2);

const int TxPin = A0;
const int RxPin = A1;

SoftwareSerial mySerial(RxPin, TxPin);

// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the first motor!
void forwardstep1() {  
  motor1.onestep(FORWARD, SINGLE);
}
void backwardstep1() {  
  motor1.onestep(BACKWARD, SINGLE);
}
// wrappers for the second motor!
void forwardstep2() {  
  motor2.onestep(FORWARD, SINGLE);
}
void backwardstep2() {  
  motor2.onestep(BACKWARD, SINGLE);
}

// Motor shield has two motor ports, now we'll wrap them in an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

const int pos1 = 4000;
const int pos2 = 0;

char readChar;

void setup()
{  
    //control 
    Serial.begin(9600);
    while(!Serial)
      ;
    
    stepper1.setMaxSpeed(600.0);
    stepper1.setAcceleration(100.0);
    stepper1.moveTo(pos1);
    
    stepper2.setMaxSpeed(600.0);
    stepper2.setAcceleration(100.0);
    stepper2.moveTo(pos1);
    
}

void loop()
{
    // Change direction based on whether pins are set to high or low
    // input pins are from Arduino Mega Master
    if(mySerial.available()) {
      readChar = mySerial.read();
      if (digitalRead(stepper1in) == HIGH) {
          stepper1.moveTo(pos1);
      }
      else {
          stepper1.moveTo(pos2);
      }
      
      if (digitalRead(stepper2in) == HIGH) {
          stepper2.moveTo(pos1);
      }
      else {
          stepper2.moveTo(pos2);
      }
    }
    
}
