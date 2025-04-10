
### This project provides code for Arduino UNO microcontroller that steers a Truck.


## Components:
- [Arduino UNO](https://docs.arduino.cc/hardware/uno-rev3/) microcontroller
- wheels and DC motors
- [L298 Motor Driver Module](https://components101.com/modules/l293n-motor-driver-module)
- [full-bridge driver L298](https://www.st.com/en/motor-drivers/l298.html)
- [I2C-LCD 16x2](https://www.amazon.com/i2c-1602-lcd/s?k=i2c+1602+lcd) + converter


## Current state:
- exemplary main.ino contains simple loop with serial communication that enables steering the truct
- Wheels class enables control of the movement
- Wheels adapts LCD dashboard and movement animations
- wip: interrupts and more robust way of measuring trucs speed