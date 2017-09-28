#include <avr/eeprom.h>
#include <avr/boot.h>

#define TYPE_SIG        0xA0 // Denotes type of device this code is intended for (position 0)
#define TYPE_LOC        0
#define SN_SIG          0x11 // Denotes serial number of device this FW is intended for (position 1)
#define SN_LOC          1

void setup() {
  Serial.begin(9600);
  Serial.print("I am type/serial flasher: ");
  Serial.print(boot_signature_byte_get(0x00),HEX); Serial.print(" ");
  Serial.print(boot_signature_byte_get(0x02),HEX); Serial.print(" ");
  Serial.println(boot_signature_byte_get(0x04),HEX);

  
  Serial.println("Current EEPROM (30 bytes): ");
  for (uint8_t i = 0; i<30; i++) {
    byte b = eeprom_read_byte((uint8_t*)i);
    Serial.print(i); Serial.print(":");Serial.println(b,HEX);
  }
  
  byte cur_type = eeprom_read_byte((uint8_t*)TYPE_LOC);
  Serial.print("Current type: "); Serial.println(cur_type,HEX);
  if (cur_type != TYPE_SIG) {
    eeprom_update_byte((uint8_t*)TYPE_LOC,TYPE_SIG);
    cur_type = eeprom_read_byte((uint8_t*)TYPE_LOC);
    Serial.print("Updated to "); Serial.println(cur_type,HEX);
  } else {
    Serial.println("Not updated - matches already");
  }
  
  byte cur_sn = eeprom_read_byte((uint8_t*)SN_LOC);
  Serial.print("Current serial: "); Serial.println(cur_sn,HEX);
  if (cur_sn != SN_SIG) {
    eeprom_update_byte((uint8_t*)SN_LOC,SN_SIG);
    cur_sn = eeprom_read_byte((uint8_t*)SN_LOC);
    Serial.print("Updated to "); Serial.println(cur_sn,HEX);
  } else {
    Serial.println("Not updated - matches already");
  }
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
    
  Serial.println("DONE - PLEASE UNPLUG ME");  

  delay(10000);
}

void loop() { 
    // Nothing to do, just loop
    noInterrupts();
    for(;;) { ; }
}
