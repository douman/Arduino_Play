/*
  Gallery_gun
  written by drm 20150717
  "Fires" serial number (GID/Gun ID) at a set rate when digital input is shorted to ground
  Debug info sent out on UART at 115200 baud
 */
const char *version="Gallery_gun -> V0.1beta-20150717";

#include <avr/wdt.h>
#include <EEPROM.h>

const byte STATUS_LED = 13; // LED to light up with each trigger press
const byte IR_LED = 9; // PWM modulated output for firing IR LED
const byte DIAG_INPUT = 3; // digital input for diagnosing PWM (timing)
const byte GID = 'A'; // Identifier for this specific gun
const byte TRIGGER = 12; // digital input for trigger signal
const byte RATE = 8; // repeat rate in Hz
const int BUFSZ = 20; // number of deltas to store in circular buffer

// state captures by the ISR
unsigned int last_time=0, i_time, deltas[BUFSZ], wrap=0;
int i_pin, i_deltas=0;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  drm_Start_print();
  
  pinMode(TRIGGER, INPUT_PULLUP);
  pinMode(DIAG_INPUT, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);

// Start with outputs off
  pinMode(IR_LED, OUTPUT);
  if(IR_LED == 9)
  {
    TCCR1A = TCCR1A;
    TCCR1B = TCCR1B;
  }
  digitalWrite(STATUS_LED, LOW);
// no watchdog for now  watchdogSetup();

// turn on interupts for the diag input
  attachInterrupt(digitalPinToInterrupt(DIAG_INPUT), pinIsr, CHANGE);
}

byte trig_state = HIGH;
unsigned long start, hi_start, hi_end;
// put your main code here, to run repeatedly:

void loop() {
/*   // trigger based loop
  if((trig_state = digitalRead(TRIGGER))==LOW)
  {
    start = millis();
    digitalWrite(LED, HIGH); // light the LED
    
    digitalWrite(LED, LOW);
  }
 */
  // Diag testing to get PWM going
  hi_start=micros();
  digitalWrite(STATUS_LED, HIGH);
  analogWrite(IR_LED, 128);
  delay(50);
//  delayMicroseconds(31000);
  digitalWrite(STATUS_LED, LOW);
  analogWrite(IR_LED, 0);
  hi_end=micros();
  prtclearBuf();
  delay(800);
//  delayMicroseconds(31000);
/*  
  prtclearBuf();
 */
  Serial.println(hi_end - hi_start);
}
void prtclearBuf()
{
  Serial.print(TCCR0A,HEX);
  Serial.print(" -0- ");
  Serial.print(TCCR0B,HEX);
  Serial.print("    ");
  Serial.print(TCCR1A,HEX);
  Serial.print(" -2- ");
  Serial.print(TCCR1B,HEX);
  Serial.print("    ");
  Serial.print(TCCR2A,HEX);
  Serial.print(" -2- ");
  Serial.println(TCCR2B,HEX);

  Serial.print(wrap); Serial.print('/'); Serial.print(i_deltas);
  Serial.print(" 1- ");
  for(int i=0; i<BUFSZ; i++)
  {
    Serial.print(deltas[i]); Serial.print(' ');
    deltas[i]=0;
  }
  Serial.println();
  wrap=0;
  i_deltas=0;
}
  void drm_Start_print() {
  Serial.print(version); Serial.print(F(" SN#"));
  Serial.println(drm_Serialno());
  Serial.print(F("Compiled-> "));
  Serial.print(F(__DATE__)); 
  Serial.print(F(" "));
  Serial.println(F(__TIME__));
}

unsigned short drm_Serialno() {
  return(EEPROM.read(5) << 8 | EEPROM.read(6)); // combine two bytes into in serial number (drm specific)
}


void pinIsr()
{
  cli();
  i_time=micros();
  i_pin=digitalRead(DIAG_INPUT);
  sei();
  if(i_deltas>=BUFSZ) return;
/*  {
    i_deltas=0;
    wrap++;
  }
   */
  deltas[i_deltas] = i_time - last_time;
  i_deltas++;
  last_time=i_time;
}

  
  
