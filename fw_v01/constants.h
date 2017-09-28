#ifndef constants_h
#define constants_h
#include <avr/boot.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include "Arduino.h"

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG 1
// 1 - general debug
// 2 - trace
// 3 - trace more

#endif
