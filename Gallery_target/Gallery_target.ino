/*
  Gallery_target
  written by drm 20150719
  "Fires" serial number (GID/Gun ID) at a set rate when digital input is shorted to ground
  Debug info sent out on UART at 115200 baud
 */
#include <avr/wdt.h>
#include <EEPROM.h>

const byte STATUS_LED = 13; // LED to light up with each detected GID press
const byte IR_SENSOR = 12; // Digital Input pin for IR Sensor 12 is nonesense for now
const byte TID = 'a'; // Identifier for this specific target
const byte DEAD = 500; // milliseconds to be dead for after each hit

const char *version="Gallery_target -> V0.1beta-20150717";

// the setup routine runs once when you press reset:
void setup() {
  pinMode(STATUS_LED, OUTPUT);
  pinMode(IR_SENSOR, INPUT_PULLUP);
  
  digitalWrite(STATUS_LED, LOW); // make sure the LED is off
  watchdogSetup();
  // initialize serial communication at 115220 bits per second for debugging
  Serial.begin(115200);
  Serial.print(version); Serial.print(" - SN#"); Serial.println(drm_serialno());  
}

short hit_counts[26];
unsigned long start;
// put your main code here, to run repeatedly:
void loop() {
 
  if((active = digitalRead(TRIGGER))==LOW)
  {
    start = millis();
    digitalWrite(LED, HIGH); // light the LED
    
    digitalWrite(LED, LOW);
  }
  
}
