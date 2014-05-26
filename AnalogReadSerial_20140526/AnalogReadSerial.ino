/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
  This example code is in the public domain.
  modified by drm 20140525 to learn Arduino
 */
#include <avr/wdt.h>

const unsigned long msec_delay=1000;

// the setup routine runs once when you press reset:
void setup() {
  watchdogSetup();
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
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
}

// the loop routine runs over and over again forever:
unsigned long i=0;
char str[10];
unsigned long usec_time=0, sec=0, frac=0, target;
long delta=0;
long adj_ms=0;
// unsigned int adj_us=0;
int adj_us=0; 

void loop() {
  wdt_reset();
  Serial.print(i); Serial.print("/"); Serial.print(adj_ms); Serial.print("/"); Serial.print(adj_us); Serial.print(": ");
  
  // read and print the input on analog pin 0:
  int sensorValue = analogRead(A0);
  Serial.print("A{0,1,2,3}-> \t{\t"); Serial.print(sensorValue); Serial.print("\t ");

  // read and print the input on analog pin 1:
  sensorValue = analogRead(A2);
  Serial.print(sensorValue); Serial.print("\t ");

  // read and print the input on analog pin 2:
  sensorValue = analogRead(A2);
  Serial.print(sensorValue); Serial.print("\t ");

  // read and nprint the input on analog pin 3:
  sensorValue = analogRead(A3);
  Serial.print(sensorValue); Serial.print("\t} --\t");

  usec_time = micros();
  sec = usec_time/1000000;
  frac = usec_time - sec * 1000000;
  frac = 1000+frac/1000;
  Serial.print(sec);
  Serial.print(".");
  sprintf(str,"%4d",frac);
  Serial.print(str+1);
  Serial.print(" sec delta-> ");
  
  target = i * msec_delay;
  delta = target*1000 - usec_time;

  adj_ms = adj_ms + delta/1000;
  adj_us = adj_us + delta;
  do {adj_ms--; adj_us=adj_us+800;} while (adj_us<0);
  Serial.println(delta);

  // delay in between reads for stability
  delay(msec_delay + adj_ms);
  int d;
  for (d=adj_us; d>0; d=d-16000) delayMicroseconds(min(d,16000));  
  i++;
}
