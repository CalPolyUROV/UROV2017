// MultiStepper
// -*- mode: C++ -*-
//
// Control both Stepper motors at the same time with different speeds
// and accelerations. 
// Requires the AFMotor library (https://github.com/adafruit/Adafruit-Motor-Shield-library)
// And AccelStepper with AFMotor support (https://github.com/adafruit/AccelStepper)
// Public domain!

#include <SoftwareSerial.h>
#include <AccelStepper.h>
#include <AFMotor.h>


// two stepper motors one on each port
AF_Stepper motor1(200, 1);
AF_Stepper motor2(200, 2);

const int TxPin = A0;
const int RxPin = A1;
const int s1Pin1 = A0;
const int s1Pin2 = A1;

//SoftwareSerial mySerial(RxPin, TxPin);

const int pos1s1 = 10000;
const int pos2s1 = -10000;
const int pos1s2 = 10000;
const int pos2s2 = -10000;

char readChar1, readChar2;

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

void setup()
{  
    //control 
/*    mySerial.begin(9600);
    while(!mySerial.isListening())
      ;*/
    Serial.begin(9600);
    while(!Serial)
      ;
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);
    stepper1.setMaxSpeed(300.0);
    stepper1.setAcceleration(1000.0);
    stepper1.setCurrentPosition(0);
    stepper1.moveTo(pos1s1);
    
    stepper2.setMaxSpeed(300.0);
    stepper2.setAcceleration(1000.0);
    stepper2.setCurrentPosition(0);
    stepper2.moveTo(pos1s2);
    Serial.println("Setup done");
}

void loop()
{
    // Change direction based on whether pins are set to high or low
    // input pins are from Arduino Mega Master
    /*if(mySerial.available()) {
      Serial.println("Got data");
      readChar1 = mySerial.read();
      Serial.println(readChar1);
      if (readChar1 == '0') {
          stepper1.moveTo(pos1s1);
          stepper1.run();
      }
      else if (readChar1 == '1') {
          stepper1.moveTo(pos2s1);
          stepper1.run();
      }
      if (readChar1 == 'a') {
          stepper2.moveTo(pos1s2);
          stepper2.run();
      }
      else if (readChar1 == 'b') {
          stepper2.moveTo(pos2s2);
          stepper2.run();
      }
      
    }*/
    if(digitalRead(A0) == HIGH && digitalRead(A1) == LOW) {
      stepper1.moveTo(1000000);
      stepper1.run();
    }
    else if(digitalRead(A1) == HIGH && digitalRead(A0) == LOW) {
      stepper1.moveTo(-1000000);
      stepper1.run();
    }
    else {
      stepper1.setSpeed(0);
    }

    if(digitalRead(A2) == HIGH && digitalRead(A3) == LOW) {
      stepper2.moveTo(1000000);
      stepper2.run();
    }
    else if(digitalRead(A3) == HIGH && digitalRead(A2) == LOW) {
      stepper2.moveTo(-1000000);
      stepper2.run();
    }
    else {
      stepper2.setSpeed(0);
    }
    
    delay(20);
}
