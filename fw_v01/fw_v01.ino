#include "constants.h"

// Define key variables that will control Arduino behaviour

#define TYPE_SIG        0xA0 // Denotes type of device this code is intended for (position 0)
#define TYPE_LOC        0
#define SN_SIG          0x11 // Denotes serial number of device this FW is intended for (position 1)
#define SN_LOC          1

#define FW_SIG          0x12 // Denotes FW version

#define PWM_FREQ        4  // PWM frequency in Hz

#define PWM_MAX         9999U // 4 digit resolution

// PWM signal will be output on pins D9 and D10 (timer 1)
#define TIMER1_DDR      DDRB
#define TIMER1_PORT     PORTB
#define TIMER1_A_PIN    B00000010       //D9
#define TIMER1_B_PIN    B00000100       //D10

// LEDs will be on timer 2 (pin 3 - green, pin 11 - red)
#define LED_GREEN_PORT    PORTD 
#define LED_GREEN_DDR     DDRD
#define LED_GREEN_PIN     B00001000

#define LED_RED_PORT      PORTB 
#define LED_RED_DDR       DDRB
#define LED_RED_PIN       B00001000

static uint16_t current_pwm = 0;        // Holds current pwm setting (00000-65535)
static uint8_t status_id = 0;           // 0 - OFF, 1 - ON
static uint16_t t1_topcnt = 0;          // Holds current timer TOP count limit
static String input_str = "";
static byte type = 0;
static byte serial = 0;

void setup() {
  prepLEDs();
  // Start serial port at 9600 bps
  Serial.begin(9600);
  // Print greeting
  #if (DEBUG)
    Serial.print("I am HeaterController: ");
    Serial.print(boot_signature_byte_get(0x00),HEX); Serial.print(" ");
    Serial.print(boot_signature_byte_get(0x02),HEX); Serial.print(" ");
    Serial.println(boot_signature_byte_get(0x04),HEX);
    Serial.print("Type: "); Serial.println(getEEPROMval(TYPE_LOC),HEX);
    Serial.print("FW: "); Serial.println(FW_SIG,HEX);
    Serial.print("SN: "); Serial.println(getEEPROMval(SN_LOC),HEX);
    Serial.print("Freq: "); Serial.println(F_CPU);
  #endif
  // Make sure this is the right board for current FW
  checkEEPROMvals();
  // Setup PWM but don't start it
  setupTimer1();
  // Start sending advertisements
  establishContact();
}

void loop() {  
  bool cmdend = false;
  if(Serial.available()) {
    while(Serial.available() > 0) {
      int nextchar = Serial.read();
      //Serial.println((char)nextchar);     
      if(nextchar == '|' || nextchar == '\n') {
        cmdend = true;
        break;
      }
      input_str += (char)nextchar;
    }
    if (cmdend) {  
      #if (DEBUG>1)  
        Serial.print("Command: ");
        Serial.println(input_str);
      #endif
      if (input_str.length() != 6) {
        Serial.println("BAD CMD LEN");
      } else {
        interpretCommand(input_str);
      }
      input_str = "";
    }
  }
}

static inline void prepLEDs() {
  LED_GREEN_PORT &= ~(LED_GREEN_PIN);
  LED_GREEN_DDR |= LED_GREEN_PIN;
  LED_RED_PORT &= ~(LED_RED_PIN);
  LED_RED_DDR |= LED_RED_PIN;
}

// Sends the hardware key on serial until receiving anything back
// If answer is not the same key, error out and shutdown
static inline void establishContact() {  
  String key = String("HC")+String(type,HEX)+String(serial,HEX);
  key.toUpperCase();
  while (Serial.available() == 0) {  
    LED_GREEN_PORT ^= LED_GREEN_PIN; //Toggle LED (i.e. blink)
    Serial.println(key); // keep sending signature
    delay(500);
  }
  // Attempt to read first command (with 1s default timeout)
  String grt = Serial.readStringUntil('\n');
  if (!grt.equals(key)) {
    Serial.println("BAD GREETING"); 
    panic();
  } else {
    Serial.println("RDY");
    grn_on();
  }
  Serial.flush();
}

// Interprets the commands received
// (man I wish i could use jumps)
static inline void interpretCommand(String cmd) {
  // Set cycle - SC#### with values 0000-9999
  // 0000 -> pwm off
  // 9999 -> fully on
  if (cmd.startsWith("SC")) {
    String value = cmd.substring(2);
    if (!isDigit(cmd.charAt(2)) || !isDigit(cmd.charAt(3)) || !isDigit(cmd.charAt(4)) || !isDigit(cmd.charAt(5))) {
      Serial.println("BAD SC NUMBER");
    } else {
      // Cast immediately since only 4 characters so can't overflow
      uint16_t valint = (uint16_t)value.toInt();
      current_pwm = valint;
      updatePWM();
      //Serial.print("SC: "); 
      //Serial.println(current_pwm);
      Serial.println("OK");
    }    
  }  
  
  // ON0000 - turns the pwm output on according to current SC set value
  else if (cmd.equals("ON0000")) {
    //startTimer1();
    status_id = 1;
    updatePWM();
    Serial.println("OK");
  }
  
  // OFF000 - turns the pwm output off and sets pins low
  else if (cmd.equals("OFF000")) {
    stopTimer1();
    setTimer1PinsOff();
    status_id = 0;
    updatePWM();
    Serial.println("OK");
  }  
  
  // QC???? - query current cycle setting, return a number with newline termination
  else if (cmd.equals("QC????")) {
    Serial.println(current_pwm);  
  }

  // QS???? - query current ON/OFF status, returns a number (0 or 1) with newline termination 
  else if (cmd.equals("QS????")) {
    Serial.println(status_id);  
  } 
  
  // TYPE?? - query type value stored in EEPROM
  else if (cmd.equals("TYPE??")) {
    Serial.println(type,HEX);  
  }

  // SN???? - query SN value stored in EEPROM
  else if (cmd.equals("SN????")) {
    Serial.println(serial,HEX);  
  }
  
  else {
    Serial.println("BAD CMD");
  }
}

// Shorthand inline functions to change LEDs
static inline void red_off() {
  LED_RED_PORT &= ~(LED_RED_PIN);
}
static inline void red_on() {
  LED_RED_PORT |= LED_RED_PIN;
}
static inline void grn_off() {
  LED_GREEN_PORT &= ~(LED_GREEN_PIN);
}
static inline void grn_on() {
  LED_GREEN_PORT |= LED_GREEN_PIN;
}


static inline void updatePWM() {
  if (current_pwm == PWM_MAX) {
    // Only set high if we are on
    if (status_id) {      
      setTimer1PinsHigh();
      stopTimer1(); 
      red_on(); 
    } else {
      red_off();    
    }
  } else if (current_pwm == 0) {
    if (status_id) {
      setTimer1PinsLowOut();
      stopTimer1();
      red_off();  
    } else {
      red_off(); 
    }
  } else {
    // Restart timer in case coming from 0/MAX
    if (status_id) {
      startTimer1();
      red_on();
    } else {
      red_off();
    }
  }
  analogWriteT1Raw(current_pwm);
}

// Code to run for permanent shutdown
static inline void panic() {
  #if (DEBUG)
    Serial.println("PANIC TRIGGERED");
  #endif 
  // Finish writing 
  Serial.flush();
  // Ensure nothing can break us out
  noInterrupts();
  // Indicate error condition
  grn_off();
  red_on();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // Turn off outputs
  stopTimer1();
  setTimer1PinsOff();
  // bye bye
  for(;;) { ; }
}


