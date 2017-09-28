# BBDuino (Black Body Duino)
This repository contains .ino files and documentation for the simple BB PWM controller based on ATMega328P or similar microcontrollers.

Typical use caseof BBDuino is supplying a PWM signal at 5V or another voltage(via MOSFET) and 16-bit resolution to a heater resistor connected to black body equivalent material somewhere in a cryostat. This heating varies the amount of photons incident on the sample or detector. Basically...science!

This firmware communicates via somewhat reliable serial (i.e. ACKs) communication protocol, and is typically controlled by ADRNX cryostat framework via HCLib.

### Flashing clean chips:
* Install latest arduino IDE
* Flash serial setting firmware - this will write values like serial number to EEPROM for future use
* Flash actual firmware

### Updating
* Unless major version changes happened, just reflash actual firmware
