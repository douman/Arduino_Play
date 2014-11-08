/*
  AnalogReadSerial
  With a watchdog 
  
  modified by drm 20140525 to learn Arduino
 */
#include <avr/wdt.h>

const unsigned long msec_delay=200;
const int led = 13;
const char *version="AnalogReadSerial_20141004 -> V1.0.0-20141018";

const unsigned int  num_ain=6;
int achs[num_ain] = {A0, A1, A2, A3, A4, A5};
/* const unsigned int  num_ain=2;
int achs[num_ain] = {A0, A5}; */
int ain[num_ain];    

// the setup routine runs once when you press reset:
void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);  
  watchdogSetup();
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println(version);
}

void watchdogSetup(void)
{
  cli();
  wdt_reset();
  /*
  WDTCSR configuration:
  WDIE = 1: Interrupt Enable
  WDE = 1 :Reset Enable
  See table for time-out variations:
  WDP3 = 0 :For 1000ms Time-out
  WDP2 = 1 :For 1000ms Time-out
  WDP1 = 1 :For 1000ms Time-out
  WDP0 = 0 :For 1000ms Time-out
  
  WDP3 = 1 :For 4000ms Time-out
  WDP2 = 0 :For 4000ms Time-out
  WDP1 = 0 :For 4000ms Time-out
  WDP0 = 0 :For 4000ms Time-out*/
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
  sei();
  delayMicroseconds(100);
}

// the loop routine runs over and over again forever:
char str[12];
unsigned long usec_time=0, sec=0, frac=0, target, wrap=0;
long delta=0;
long adj_ms=0;
long adj_us=20000, adj_us2=1000;
unsigned long i=0;
long avg_delta=0;
  
void loop() {

  wdt_reset();
  
  if(wrap>0) {Serial.print(wrap); Serial.print("-wrap-");}
  
  Serial.print(i); 
  Serial.print(": ");

  usec_time = micros();
  sec = usec_time/1000000;
  frac = usec_time - sec * 1000000;
  unsigned int ms_frac = 1000 + frac/1000; /* convert to msec + 1000 */
  Serial.print(sec);
  Serial.print(".");
  sprintf(str,"%4d",ms_frac);
  Serial.print(str+1); /* strip off the leading 1 to get msecs */

  char istat = read_ain_print();
  
  target = i * msec_delay * 1000;
  delta = (long) usec_time - (long) target;
  avg_delta = (3*avg_delta)/4 + delta/4;

  if(abs(delta) > 600) {
    if(delta > 0) {
      adj_us += 999;
    }
    else if(delta < 0) {
      adj_us -= 888;
    }
  }

  if(avg_delta > 0 && adj_us2 > 1) 
    adj_us2 -= 13;
  else if (avg_delta < 0)
    adj_us2+=11;

  if(abs(adj_us2-1000) > 1000) adj_us2 = 1000;

  delay(msec_delay - ((delta + adj_us)/1000));
  delayMicroseconds(adj_us2);

  Serial.print("  adj_us-> "); Serial.print(adj_us); Serial.print("/"); Serial.print(adj_us2);
  Serial.print("  avg_delta-> "); Serial.print(avg_delta);
  Serial.print("  Delta-> "); Serial.print(delta); Serial.print(" usec");
  Serial.println(""); 
    
  i++;
}

char read_ain_print() {
  int i;

  // read and print the input on analog pin 0:
  digitalWrite(led, HIGH);  
  for (i=0; i<num_ain; i++) {
    ain[i] = analogRead(achs[i]);
  }
  digitalWrite(led, LOW);  
  
  Serial.print("--> A{");
  Serial.print(achs[0]);
  for (i=1; i<num_ain; i++) {Serial.print(","); Serial.print(achs[i]);}
  Serial.print(")-> (");
  for (i=0; i<num_ain; i++) {Serial.print(ain[i]); Serial.print(" ");}
  Serial.print(")");
}
