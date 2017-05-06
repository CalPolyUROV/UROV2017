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

//#include "ComsMasterArd.h"
//#include "pressure.h"
//#include "QuadMotorShields.h"
//#include "currentSensing.h"

//#include "Adafruit_Sensor.h"
//#include "Adafruit_BNO055.h"
//#include "utility/imumaths.h"

//Declarations for temp readings
//#include <OneWire.h>

//#include "DallasTemperature.h"

// Data wire is plugged into pin 26 on the Arduino
#define ONE_WIRE_BUS 3

//Serial baud rate, match with top side python's baud rate
#define BAUD_RATE 9600

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
//DallasTemperature sensors(&oneWire);

//Adafruit_BNO055 bno = Adafruit_BNO055();



//all pins used must be listed here! either as a variable to change quickly later or as a comment if it is in another file

/* this is the pin to control whethgeter it is recieving or sending
   SDA, SCL to Slave arduino */
int serialWritePin = 2;


///// Pins used by Quad Motor Shields //////
//
// 7, 6, 11, 12, 22, 24, 31, 33, 29, 25, 27, 23, A1,
//42, 44, 40, 24, 26, 22
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
bool accel = false;
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

//SoftwareSerial Ser3(14, 15);
void setup() {
  /*Serial 3: Communication to topside python code through rs485, match baud rate in python code*/
  /*Pins 14 & 15*/
  Serial3.begin(BAUD_RATE);
  while(!Serial3) {
    ;
  }
  /*Serial: Communication to Arduino Serial Console*/
  /*Pins 0 and 1*/
  Serial.begin(BAUD_RATE);     //it does not seem to work at lower baud rates
  while(!Serial) {
    ;
  }
  pinMode(serialWritePin, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);

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

  Serial.println("Finished setup");

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
  while (true) {
    Serial.println("Waiting");
    wait();
    Serial.println("Recieved");
    if ('S' == Serial3.read()) {
      Serial.println("Read S");
      wait();
      if ('T' == Serial3.read()) {
        Serial.println("Read T");
        wait();
        if ('R' == Serial3.read()) {
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
  return input;
}

/*Currently sets camera relay*/
void processInput(Input i) {

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

  setCameras(i.buttons1);
  setMotors(i.primaryX, i.primaryY, i.triggers, i.secondaryX, i.buttons1);
}

void writeToCommand(Input i) {
  Serial3.print("STR");
  int lines = 0;
  //if (pressure) lines += 2;
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
    //Serial3.println("PSR"); //tell it the next line is Pressure
    //coms.sendSlaveCmd(GET_PRES);
    //Serial3.print(coms.getSlaveData());
    //Serial3.println(" mbars");
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

  bno.getEvent(&event);
  pch = event.orientation.y;
  yaw = event.orientation.x;
  rol = event.orientation.z;
  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accelData[0] = linearaccel[0];
  accelData[1] = linearaccel[1];
  accelData[2] = linearaccel[2];

  if (accel) {
    Serial3.println("ACL"); //tell it the next line is Accelerometer
    Serial3.print("Accel: X: ");
    Serial3.print(accelData[0]);
    Serial3.print(" Y: ");
    Serial3.print(accelData[1]);
    Serial3.print(" Z: ");
    Serial3.print(accelData[2]);
    Serial3.print("\nGyro: X: ");
    Serial3.print(yaw);
    Serial3.print(" Y: ");
    Serial3.print(pch);
    Serial3.print(" Z: ");
    Serial3.print(rol);
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
  //if (Serial3.available()) {
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
  /*}
  else {
    Serial3.println("Not writing to surface");
  }*/
}

