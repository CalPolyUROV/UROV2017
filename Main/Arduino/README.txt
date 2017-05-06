This is the directory for the final firmware code.

Changes to be made/Stuff to keep for 2017 robot:

FIRMWARE:

   Current #includes in the main Arduino.ino file:

   #include "VectorMotors.h"
   #include "cameras.h"
   #include "Math.h"
   #include "Accelerometer.h"  <------------ These functions are not used at all
   #include "dataStruc.h"
   #include "gyroAccelerometer.h"
   #include <OneWire.h>
   #include "DallasTemperature.h"

   Change:
      -Accelerometer.cpp, gyroAccelerometer.cpp and Adafruit_BNO055.cpp (and their respective .h files) will be replaced with Spencer's new Accelerometer code
      -Code for external temperature sensor will be added, old imu stuff will be removed
      -Adafruit sensor.h - Why is this file here in the first place?
      -ComsMasterArd coms is used for slave Arduino I2C stuff in last year's code, add in only if needed
      -remove pressure sensor code, not needed this year
      

   Keep:
      -Vector Motor Driver code will be kept as is
      -Serial Communications method works and can be used still
      
      
   Pins:
        serial - upsidedown cat 6 ltr
        2 - 1 - (unused brown)
        15 - 2 - brown stripe
        3 - 3 - green
        14 - 4 - blue stripe - GND
        1 - 5 - blue
        16 - 6 - green stripe
        4 - 7 - orange - TX3
        17 - 8 - o stripe - RX3
