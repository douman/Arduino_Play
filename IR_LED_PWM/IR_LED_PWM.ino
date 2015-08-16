/* Code to pulse pin 3 with a modulated signal
* Can be used to drive an IR LED to keep a TSOP IR reciever happy
* This allows you to use a modulated reciever and a continious beam detector
* By Mike Cook Nov 2011 - Released under the Open Source licence
*/
volatile byte pulse = 0;

ISR(TIMER2_COMPB_vect){  // Interrupt service routine to pulse the modulated pin 3
    pulse++;
  if(pulse >= 8) { // change number for number of modulation cycles in a pulse
    pulse =0;
    TCCR2A ^= _BV(COM2B1); // toggle pin 3 enable, turning the pin on and off
  }
}

void setIrModOutput(){  // sets pin 3 going at the IR modulation rate
  pinMode(3, OUTPUT);
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20); // Just enable output on Pin 3 and disable it on Pin 11
  TCCR2B = _BV(WGM22) | _BV(CS22);
  OCR2A = 51; // defines the frequency 51 = 38.4 KHz, 54 = 36.2 KHz, 58 = 34 KHz, 62 = 32 KHz
  OCR2B = 26;  // deines the duty cycle - Half the OCR2A value for 50%
  TCCR2B = TCCR2B & 0b00111000 | 0x2; // select a prescale value of 8:1 of the system clock
}

void setup(){
  setIrModOutput();
  TIMSK2 = _BV(OCIE2B); // Output Compare Match B Interrupt Enable
}

void loop(){
// do something here
}
