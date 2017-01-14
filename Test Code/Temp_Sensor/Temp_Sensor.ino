#include <OneWire.h>
#include <DallasTemperature.h>
OneWire  ds(10);  // need 4.7k pullup
DallasTemperature sensors(&ds);
DeviceAddress temp_sensor = {0x28, 0xCF, 0xC7, 0xEA, 0x05, 0x00, 0x00, 0xB1};

void setup(void) {
  Serial.begin(9600);
  Serial.println(sensors.getResolution(temp_sensor), DEC);

}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if (!ds.search(addr)) {
    ds.reset_search();
    delay(10);
    return;
  }
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(200);     // 200ms for 10 bit resolution, set before
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // reads 9 bytes
    data[i] = ds.read();
  }

  //set it to 16 bit signed interger 
  int16_t raw = (data[1] << 8) | data[0];

  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
}
