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

SoftwareSerial Ser3(14, 15);
void setup() {
  /*Serial 3: Communication to topside python code through rs485*/
  Ser3.begin(BAUD_RATE);   //the number in here is the baud rate, it is the communication speed, this must be matched in the python
  /*Serial: Communication to Arduino Serial Console*/
  Serial.begin(BAUD_RATE);     //it does not seem to work at lower baud rates
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
    Serial.print("It's broken, check the wiring");  //its broken
  }
  bno.setExtCrystalUse(true);

  //sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12.
  //Ups the delay giving the IC more time to process the temperature measurement

  Serial.println("Made it 1");

  digitalWrite(serialWritePin, LOW);
  motorSetup();
  //if(ypr){bno.setExtCrystalUse(true);}

  Serial.println("Made it 1");

}

//looks cleaner than the empty while loop being everywhere in the code
void wait() {
  while (!Ser3.available())
    ;
}

//detects the start signal without accidently overshooting it because of short circuting logic
//except for input such as SSTR, that will be skipped. There should be multiple characters to
//prevent random bytes getting past
void waitForStart() {
  while (true) {
    wait();
    if ('S' == Ser3.read()) {
      wait();
      if ('T' == Ser3.read()) {
        wait();
        if ('R' == Ser3.read()) {
          break;
        }
      }
    }
  }
}

Input readBuffer() {
  Input input;
  wait();//makes sure that a byte of data is not missed
  input.buttons1 = Ser3.read();
  wait();
  input.buttons2 = Ser3.read();
  wait();
  input.primaryX = Ser3.parseInt();
  Ser3.read();
  wait();
  input.primaryY = Ser3.parseInt();
  Ser3.read();
  wait();
  input.secondaryX = Ser3.parseInt();
  Ser3.read();
  wait();
  input.secondaryY = Ser3.parseInt();
  Ser3.read();
  wait();
  input.triggers = Ser3.parseInt();
  Ser3.read();
  return input;
}
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
  Ser3.print("STR");
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
    Ser3.print("0");
    counter++;
  }
  Ser3.print(numberOfLines); //print the number of lines of input the python program can read in three digits
  if (pressure) {
    //Ser3.println("PSR"); //tell it the next line is Pressure
    //coms.sendSlaveCmd(GET_PRES);
    //Ser3.print(coms.getSlaveData());
    //Ser3.println(" mbars");
  }
  if (voltage) {
    Ser3.println("VLT"); //tell it the next line is Power info
    Ser3.println( (((float)(analogRead(A1)) * (5.0 / 1023.0)) - 2.52)   / .066 );
    //Ser3.println(" amps");
  }
  if (temperature) {

    //Ser3.println("TMP"); //tell it the next line is Temperature
    //coms.sendSlaveCmd(GET_TEMP);
    //sensors.requestTemperatures(); // Send the command to get temperatures
    //fast = (sensors.getTempCByIndex(0));
    //Ser3.println(fast);
  }

  bno.getEvent(&event);
  pch = event.orientation.y;
  yaw = event.orientation.x;
  rol = event.orientation.z;
  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accelData[0] = linearaccel[0];
  accelData[1] = linearaccel[1];
  accelData[2] = linearaccel[2];
  /*
    if (ypr) {

    euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

    //coms.sendSlaveCmd(GET_YAW);
    Ser3.println("YAW");
    Ser3.println((int)(euler.x()));
    //Ser3.println(coms.getSlaveData());

    //coms.sendSlaveCmd(GET_PCH);
    Ser3.println("PCH");
    Ser3.println((int)(euler.y()));

    Serial.println((int)(euler.y()));

    //coms.sendSlaveCmd(GET_ROL);
    Ser3.println("ROL");
    Ser3.println((int)(euler.z()));


    if(debug){

    //coms.sendSlaveCmd(GET_YAW);
    //Serial.println("YAW");
    //yaw = coms.getSlaveData();

    Serial.println(yaw);
    //Ser3.println(coms.getSlaveData());

    coms.sendSlaveCmd(GET_PCH);
    Serial.println("PCH");
    Serial.println(coms.getSlaveData());

    coms.sendSlaveCmd(GET_ROL);
    Serial.println("ROL");
    Serial.println(coms.getSlaveData());

    }
    }*/
  if (accel) {
    Ser3.println("ACL"); //tell it the next line is Accelerometer
    Ser3.print("Accel: X: ");
    Ser3.print(accelData[0]);
    Ser3.print(" Y: ");
    Ser3.print(accelData[1]);
    Ser3.print(" Z: ");
    Ser3.print(accelData[2]);
    Ser3.print("\nGyro: X: ");
    Ser3.print(yaw);
    Ser3.print(" Y: ");
    Ser3.print(pch);
    Ser3.print(" Z: ");
    Ser3.print(rol);
  }
  if (depth) {
    Ser3.println("DPT"); //tell it the next line is Depth
    //coms.sendSlaveCmd(GET_DEPT);
    //Ser3.print(coms.getSlaveData());
    Ser3.println(" feet");
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
  if (Ser3.available()) {
    //digitalWrite(13, HIGH);
    waitForStart();
    Input i = readBuffer();
    digitalWrite(serialWritePin, HIGH);
    if (debug == true)
      debugInput(i);
    writeToCommand(i); //this is where the code to write back to topside goes.
    Ser3.flush();
    //sensors.requestTemperatures();
    delay(50);         //this delay allows for hardware serial to work with rs485
    digitalWrite(serialWritePin, LOW);

    processInput(i);//gives the inputs to the motors
  }
}

