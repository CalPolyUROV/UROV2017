#include <Adafruit_BNO055.h>


#include <Servo.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>

#include "VectorMotors.h"
#include "cameras.h"
#include "Math.h"
//#include "Accelerometer.h"
#include "dataStruc.h"
#include "gyroAccelerometer.h"

// Data wire is plugged into pin 26 on the Arduino
#define ONE_WIRE_BUS 3

//Serial baud rate, match with top side python's baud rate
#define BAUD_RATE_UI 115200
#define BAUD_RATE_SER 9600

#define A_HEX 0x1
#define B_HEX 0x2
#define X_HEX 0x4
#define Y_HEX 0x8
#define L_TRIGGER_HEX 0x10
#define R_TRIGGER_HEX 0x20
#define BACK_HEX 0x40
#define BROKEN_HEX 0x80   //this hex signal over serial seems to lag the arduino by a second, so we are not using it, ever
#define START_HEX 0x100
#define L_JOYSTICK_CLICK_HEX 0x200
#define R_JOYSTICK_CLICK_HEX 0x400

//all pins used must be listed here! either as a variable to change quickly later or as a comment if it is in another file

/* this is the pin to control whetheter it is recieving or sending in rs485
   SDA, SCL to Slave arduino */
int serialWritePin = 2;

int solenoidPin = A5;
boolean toggleSol = false;

///// Pins used by Vector Motors //////
//
// 8, 9, 10, 11, 12, 13
//
//////////////////////////////////

///// Pins used by Pressure Sensor //////
//
//45, 50, 51, 52
//
//////////////////////////////////

//End of pin listings

///// Timers used by Quad Motor Shields on MEGA 2560//////
//
// Timer0, Timer1, Timer4
//
//////////////////////////////////

/*ComsMasterArd can be used if we need to
  communicate to slave Arduino via I2C*/
//ComsMasterArd coms;
//QuadMotorShields md;//Not being used anymore
bool pressure = false;
bool voltage = false;
bool temperature = true;
bool accel = true;
bool depth = false;
bool ypr = true;
bool amperage = false;

bool debug = false;

Adafruit_BNO055 bno = Adafruit_BNO055(55);
sensors_event_t event;
int yaw;
int pch;
int rol;
float accelData[3];

float fast;

uint16_t amperages[8] = {0};
uint16_t* p_amperages;

void startup();

void setup() {
  /*Serial 3: Communication to topside python code through rs485, match baud rate in python code*/
  /*Pins 14 & 15*/
  Serial3.begin(BAUD_RATE_UI);    //it does not seem to work at lower baud rates
  while(!Serial3) {
    ;
  }
  /*Serial: Communication to Arduino Serial Console*/
  /*Pins 0 and 1*/
  Serial.begin(BAUD_RATE_SER);   
  while(!Serial) {
    ;
  }
  /*Serial 2: Communication to slave Arduino */
  /* Pins 16 & 17*/
  Serial2.begin(BAUD_RATE_SER);
  while(!Serial2) {
    ;
  }  
  pinMode(serialWritePin, OUTPUT);
  pinMode(solenoidPin, OUTPUT);
  pinMode(13, OUTPUT);
  
  pinMode(4, OUTPUT); //4,5,6,7,22,24 have all been used for relay in the past. currently using 4,5,6,7
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  digitalWrite(4, LOW); //Relay 4
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW); //Relay 1

  if (!bno.begin()) {  //initialize the sensor
    Serial.println("Sensor not detected, check the wiring");  //its broken
  }
  bno.setExtCrystalUse(true);

  //sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12.
  //Ups the delay giving the IC more time to process the temperature measurement

  Serial.println("Made it Serial 1");
  Serial3.println("Made it Serial 3");

  digitalWrite(serialWritePin, LOW);
  motorSetup();
  //if(ypr){bno.setExtCrystalUse(true);}

  //startup();
  Serial.println("Finished setup");

}

void startup() {
  motor_1(-75);
  delay(1000);
  motor_1(75);
  delay(1000);
  motor_2(75);
  delay(1000);
  motor_2(-75);
  delay(1000);
  motor_3(75);
  delay(1000);
  motor_3(-75);
  delay(1000);
  motor_4(75);
  delay(1000);
  motor_4(-75);
  delay(1000);
  motor_5(75);
  delay(1000);
  motor_5(-75);
  delay(1000);
  motor_6(75);
  delay(1000);
  motor_6(-75);
  delay(1000);
}

//looks cleaner than the empty while loop being everywhere in the code
void wait() {
  while (!Serial3.available())
    ;
}

//detects the start signal without accidently overshooting it because of short circuting logic
//except for input such as SSTR, that will be skipped. There should be multiple characters to
//prevent random bytes getting past
void waitForStart() {
  char x;
  while (true) {
    Serial.println("Waiting");
    wait();
    Serial.println("Recieved");
    x = Serial3.read();
    Serial.println(x);
    if ('S' == x) {
      Serial.println("Read S");
      wait();
      x = Serial3.read();
      Serial.println(x);
      if ('T' == x) {
        Serial.println("Read T");
        wait();
        x = Serial3.read();
        Serial.println(x);
        if ('R' == x) {
          Serial.println("Read R");
          break;
        }
      }
    }
  }
}

Input readBuffer() {
  Input input;
  wait();//makes sure that a byte of data is not missed
  Serial.println("Reading...");
  input.buttons1 = Serial3.read();
  Serial.println("Read a byte...");
  wait();
  input.buttons2 = Serial3.read();
  wait();
  input.primaryX = Serial3.parseInt();
  Serial3.read();
  wait();
  input.primaryY = Serial3.parseInt();
  Serial3.read();
  wait();
  input.secondaryX = Serial3.parseInt();
  Serial3.read();
  wait();
  input.secondaryY = Serial3.parseInt();
  Serial3.read();
  wait();
  input.triggers = Serial3.parseInt();
  Serial3.read();
  Serial.println("Packet received");
  return input;
}

/*Currently sets camera relay*/
void processInput(Input i) {
  unsigned char step1, step2;
  if ((CHECK_BIT(i.buttons1, 3))) {
    digitalWrite(22, HIGH);
  }
  else {
    digitalWrite(22, LOW);
  }
  if ((CHECK_BIT(i.buttons1, 4))) {
    digitalWrite(24, HIGH);
  }
  else {
    digitalWrite(24, LOW);
  }
  step1 = step2 = 'x';
  digitalWrite(A0, LOW); 
  digitalWrite(A1, LOW); 
  digitalWrite(A2, LOW); 
  digitalWrite(A3, LOW); 
  switch(i.buttons1) {
    case A_HEX:
      digitalWrite(A0, HIGH);   
    break;
    case B_HEX:
      digitalWrite(A1, HIGH); 
    break;
    case X_HEX:
      digitalWrite(A2, HIGH); 
    break;
    case Y_HEX:
      digitalWrite(A3, HIGH); 
    break;
    case BACK_HEX:
      toggleSol = !toggleSol;
      digitalWrite(solenoidPin, toggleSol);
    break;
  }
  switch(i.buttons2) {
    case A_HEX:
      digitalWrite(A0, HIGH); 
    break;
    case B_HEX:
      digitalWrite(A1, HIGH); 
    break;
    case X_HEX:
      digitalWrite(A2, HIGH); 
    break;
    case Y_HEX:
      digitalWrite(A3, HIGH); 
    break;
    case BACK_HEX:
      toggleSol = !toggleSol;
      digitalWrite(solenoidPin, toggleSol);
    break;
  }
  Serial2.print(step1);
  Serial2.print(step2);
  setCameras(i.buttons1);
  setMotors(i.primaryX, i.primaryY, i.triggers, i.secondaryX, i.buttons1);
}

void writeToCommand(Input i) {
  Serial3.print("STR");
  int lines = 0;
  if (pressure) lines += 2;
  if (voltage) lines += 2;
  //if (temperature) lines += 2;
  if (depth) lines += 2;
  if (accel) lines += 4;
  //if (ypr) lines += 6;
  String numberOfLines = String(lines);
  int counter = 0;
  while ((counter + numberOfLines.length()) != 3) {
    Serial3.print("0");
    counter++;
  }
  Serial3.print(numberOfLines); //print the number of lines of input the python program can read in three digits
  if (pressure) {
    Serial3.println("PSR"); //tell it the next line is Pressure
    //coms.sendSlaveCmd(GET_PRES);
    //Serial3.print(coms.getSlaveData());
    Serial3.println(" 5");
  }
  if (voltage) {
    Serial3.println("VLT"); //tell it the next line is Power info
    Serial3.println( (((float)(analogRead(A1)) * (5.0 / 1023.0)) - 2.52)   / .066 );
    //Serial3.println(" amps");
  }
  if (temperature) {

    //Serial3.println("TMP"); //tell it the next line is Temperature
    //coms.sendSlaveCmd(GET_TEMP);
    //sensors.requestTemperatures(); // Send the command to get temperatures
    //fast = (sensors.getTempCByIndex(0));
    //Serial3.println(fast);
  }

  /*bno.getEvent(&event);
  pch = event.orientation.y;
  yaw = event.orientation.x;
  rol = event.orientation.z;
  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accelData[0] = linearaccel[0];
  accelData[1] = linearaccel[1];
  accelData[2] = linearaccel[2];*/
  pch = yaw = rol = 4;
  accelData[0] = accelData[1] = accelData[2] = 5;

  if (accel) {
    Serial3.println("ACL"); //tell it the next line is Accelerometer
    //Accel data: x, y, z
    Serial3.print(accelData[0]);
    Serial3.print(",");
    Serial3.print(accelData[1]);
    Serial3.print(",");
    Serial3.println(accelData[2]);
    
    Serial3.println("GYR");
    //Gyro data: yaw, pitch, roll
    Serial3.print(yaw);
    Serial3.print(",");
    Serial3.print(pch);
    Serial3.print(",");
    Serial3.println(rol);
  }
  if (depth) {
    Serial3.println("DPT"); //tell it the next line is Depth
    //coms.sendSlaveCmd(GET_DEPT);
    //Serial3.print(coms.getSlaveData());
    Serial3.println(" feet");
  }
}
void debugInput(Input i) {
  //the following is for debugging, prints all input back out on the serial used for programming the arduino
  Serial.print("buttons: ");
  Serial.print(i.buttons2);
  Serial.print(" ");
  Serial.print(i.buttons1);
  Serial.print(" X1: ");
  Serial.print(i.primaryX);
  Serial.print(" Y1: ");
  Serial.print(i.primaryY);
  Serial.print(" X2: ");
  Serial.print(i.secondaryX);
  Serial.print(" Y2: ");
  Serial.print(i.secondaryY);
  Serial.print(" Trig: ");
  Serial.println(i.triggers);
}

void loop() {
  if (Serial3.available()) {
    //digitalWrite(13, HIGH);
    waitForStart();
    Input i = readBuffer();
    digitalWrite(serialWritePin, HIGH);
    if (debug == true)
      debugInput(i);
    Serial.println("Writing to surface");
    writeToCommand(i); //this is where the code to write back to topside goes.
    Serial3.flush();
    //sensors.requestTemperatures();
    delay(50);         //this delay allows for hardware serial to work with rs485
    digitalWrite(serialWritePin, LOW);

    processInput(i);//gives the inputs to the motors
  }
  else {
    //Serial3.println("N"); //*ot writing to surface");
  }
}

