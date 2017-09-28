static inline void analogWriteT1Raw(uint16_t value) {
  value = (uint16_t)(((long)value*((long)t1_topcnt))/PWM_MAX);
  OCR1A = value;
  OCR1B = value;
  #if (DEBUG > 1)
    Serial.print("Timer 1 set to scaled value "); Serial.println(value);
  #endif
}

// Sets up timer1 (16-bit) with the defined frequency
static inline void setupTimer1() {
  noInterrupts();
  setTimer1PinsOff();
  unsigned long top = F_CPU / (64*2*PWM_FREQ);
  TCCR1A = TCCR1B = 0;        // clear control register A,B
  TIMSK1 = 0;                 // clear timer interrupt mask  
  TCNT1 = 0;                  // clear timer counter
  OCR1A = OCR1B = 0;          // reset compares    
  TIFR1 &= ~(_BV(OCF1A)|_BV(OCF1B)|_BV(TOV1)|_BV(ICF1)); // Clear timer interrupt flags
                 
  TCCR1B = _BV(WGM13)      // mode 8: ph. freq. correct PWM, TOP = ICR1
         | _BV(CS11)      // system clock, prescaler = 64
         | _BV(CS10);      // as such, min freq = 16MHz/8/(2*65535) = 15Hz
                           
         
  // Note that ICR is not double buffered...but we never change frequency
  ICR1 = top;              // TOP value to count to before reversing
  t1_topcnt = top;
  interrupts();
  #if DEBUG
    Serial.print("Timer 1 setup: TOP="); Serial.print(top);
    Serial.print(" and f="); Serial.println(PWM_FREQ);
  #endif
}

// Sets up timer2 for LEDs
//static void setupTimer2() {
//  noInterrupts();
//  TCCR2A = TCCR2B = 0;        // clear control register A,B
//  TIMSK2 = 0;                 // clear timer interrupt mask  
//  TCNT2 = 0;                  // clear timer counter
//  OCR2A = OCR2B = 0;          // reset compares    
//  TIFR2 &= ~(_BV(OCF2A)|_BV(OCF2B)|_BV(TOV2)); // Clear timer interrupt flags
//                 
//  TCCR2B = _BV(WGM22) | _BV(WGM20)  // mode 5: ph. freq. correct PWM, TOP = OCRA
//         | _BV(CS22) 
//         | _BV(CS21)                
//         | _BV(CS20);               // Prescale by 1024
//
//  TCCR2A = _BV(COM2A0) | _BV(COM2B1) | _BV(COM2B0);  // non-inverted PWM on ch. A/B
//  OCR2A = 255;
//  OCR2B = 25;
//  interrupts();
//  #if DEBUG
//    Serial.println("Timer 2 setup !");
//  #endif
//}

static inline void setTimer1PinsHigh() {
  TIMER1_PORT |= (TIMER1_A_PIN | TIMER1_B_PIN);  // Set HIGH
  TIMER1_DDR  |= (TIMER1_A_PIN | TIMER1_B_PIN);  // Timer pins to outputs 
}

static inline void setTimer1PinsOff() {
  TIMER1_PORT &= ~(TIMER1_A_PIN | TIMER1_B_PIN); // Set LOW
  TIMER1_DDR  &= ~(TIMER1_A_PIN | TIMER1_B_PIN); // Timer pins to inputs 
}

static inline void setTimer1PinsLowOut() {
  TIMER1_PORT &= ~(TIMER1_A_PIN | TIMER1_B_PIN); // Set LOW
  TIMER1_DDR  |= (TIMER1_A_PIN | TIMER1_B_PIN);  // Timer pins to outputs 
}

static inline void stopTimer1() {
  // Stop PWM
  #if (DEBUG > 2)
    Serial.println("Stopping timer 1");
  #endif
  TCCR1A &= ~(_BV(COM1A1)|_BV(COM1B1)); 
  //TCCR1A &= ~(_BV(COM1A1)|_BV(COM1B1)|_BV(COM1B0)); 
}

static inline void startTimer1() {
  // Start PWM
  #if (DEBUG > 2)
    Serial.println("Starting timer 1");
  #endif
  TIMER1_DDR  |= (TIMER1_A_PIN | TIMER1_B_PIN);  // Timer pins to output
  TCCR1A = _BV(COM1A1)   // non-inverted PWM on ch. A
       | _BV(COM1B1);    // non-inverted on ch. B
}
