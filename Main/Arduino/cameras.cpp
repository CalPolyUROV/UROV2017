#include "arduino.h"
#include <Wire.h>
#include <Servo.h>
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define NUM_CAMERAS 4

//camera pins
int _c1 = 7;
int _c2 = 6;
int _c3 = 5;
int _c4 = 4;
int currentCamera = 1;
bool debounce = 0;

void setCameras(unsigned char buttons)
{
    
    if(CHECK_BIT(buttons, 4)&&debounce){
        debounce = 0;
        //digitalWrite(13, HIGH);
        currentCamera=(currentCamera << 1);
        if(currentCamera==(1<<(NUM_CAMERAS))) {currentCamera = 1;}
        
        digitalWrite(7,!(CHECK_BIT(currentCamera, 0)));
        digitalWrite(6,CHECK_BIT(currentCamera, 1));
        digitalWrite(5,CHECK_BIT(currentCamera, 2));
        digitalWrite(4,CHECK_BIT(currentCamera, 3));
       
    }
    else {
      debounce = 1;
        //digitalWrite(13, LOW);
    }
    
    
    
}
