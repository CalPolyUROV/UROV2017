#include <OneWire.h>

OneWire ds(10);

void tempSetup() { //sets resolution to 9 bits
    byte addr[8];

    ds.reset();
    ds.search(addr);

    if(addr[0] == 0x20) {
        ds.reset();
        ds.select(addr); // selects address
        ds.write(0x4E);  // writes on scratchpad mem.
        ds.write(0x00);  // first user byte is 0
        ds.write(0x00);  // second user byte is 0
        ds.write(0x1F);  // set 9 bits, see data sheet for hex number

        ds.reset();      // reset 1-wire
        ds.select(addr); // select DS18b20

        ds.write(0x48);  // copy to EEPROM
        delay(15);       // wait a little to finish writing
    }
}

float readTemp() {
    byte i;
    byte data[12];
    byte addr[8];
    float celsius, fahrenheit;

    if(!ds.search(addr)) { //resets onewire if address not found 
        ds.reset_search();
        delay(50);
    }
	   
	ds.reset();        //resets onewire
	ds.select(addr);
	ds.write(0x44, 1); //starts conversion, 1 if using 1 wire

	delay(95);		  // 93.75 ms for 9 bit resolution

	ds.reset();
    ds.select(addr);   //select address
	ds.write(0xBE);    //read data

    for(i = 0; i < 9; i++) {
	   data[i] = ds.read();
	}

	int16_t raw = (data[1] << 8) | data[0]; // set to 16 bit signed int

	celsius = (float)raw / 16.0;      
	fahrenheit = celsius * 1.8 + 32.0;

	return fahrenheit;
}
