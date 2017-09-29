## BBDuino (Black Body Duino)
This repository contains .ino files and documentation for the simple BB PWM controller based on ATMega328P.

Typical use case of BBDuino is supplying a PWM signal at 5V or another voltage(via MOSFET) and 16-bit resolution to a resistor thermally linked to black body 'equivalent' material somewhere in a cryostat. This heating varies the amount of photons incident on sample/detector, allowing extraction of important performance metrics. Basically...science!

Firmware communicates via somewhat reliable serial (i.e. has ACKs) communication protocol. Protocol specification and tests are provided in HCLib, a part of ADRNX cryostat automation framework (with which BBDuino is typically controlled in our setup).

### Basic circuit (only power elements are shown, no status LEDs)
![Alt text](/docs/circuit_diagram.png?raw=true "LTSpice circuit diagram")

### Flashing clean boards:
* Install latest arduino IDE
* Flash serial setting firmware (fw_typeandserial.ino) - this will write specified serial number and type to EEPROM as safeguard against running wrong FW type
* Flash actual firmware of correct version (fw_v[xx].ino)

### Updating
* Check board output on startup to see current type, and flash appropriate FW version
