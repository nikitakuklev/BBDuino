// Retrieves value at specified EEPROM address
static inline byte getEEPROMval(uint8_t number) {
  return eeprom_read_byte((uint8_t*)number);
}

//Checks if expected/actual type and SN values match
static  void checkEEPROMvals() {
  #if (DEBUG)
     Serial.print("Checking EEPROM...");
  #endif
  type = getEEPROMval(TYPE_LOC);
  serial = getEEPROMval(SN_LOC);
  if (type == TYPE_SIG) {
    #if (DEBUG)
     Serial.print("TYPE OK...");    
    #endif
    if (serial == SN_SIG) {
      #if (DEBUG)
        Serial.println("SN OK!");    
      #endif
    } else {
      #if (DEBUG)
        Serial.print("SN MISMATCH! WANT:"); Serial.println(SN_SIG,HEX); 
      #endif
      panic();
    }
  } else {
    #if (DEBUG)
     Serial.print("TYPE MISMATCH! WANT:"); Serial.println(TYPE_SIG,HEX);
    #endif
    panic();
  }
}
