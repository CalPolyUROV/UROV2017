BNO055  9 Axis Abs. Orientation
3 Axis Accel+Gyro+Mag w/ Quaternion, Euller Heading, etc. I2C out

Data sheets:
https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf
http://www.mouser.com/new/bosch-sensortec/bosch-bno55-sensor/
http://www.mouser.com/ds/2/621/BST_BNO055_DS000_10_Release-371197.pdf

Overview:
https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview

Pinout from overview:
Power Pins
VIN: 3.3-5.0V power supply input
3VO: 3.3V output from the on-board linear voltage regulator, you can grab up to about 50mA as necessary
GND: The common/GND pin for power and logic
I2C Pins
SCL - I2C clock pin, connect to your microcontrollers I2C clock line. This pin can be used with 3V or 5V logic, and there's a 10K pullup on this pin.
SDA - I2C data pin, connect to your microcontrollers I2C data line. This pin can be used with 3V or 5V logic, and there's a 10K pullup on this pin.

Current pin setup:
BNO055	Wire	UNO
Vin		blue	3.3v
GND		orange	GND
SDA		yellow	A4
SCL		brown 	A5