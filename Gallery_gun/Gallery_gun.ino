/*
  Gallery_gun
  written by drm 20150717
  "Fires" serial number (GID/Gun ID) at a set rate when digital input is shorted to ground
  Debug info sent out on UART at 115200 baud
 */
#include <avr/wdt.h>
#include <EEPROM.h>

const byte STATUS_LED = 13; // LED to light up with each trigger press
const byte IR_LED = xx; // PWM modulated output for firing IR LED
const byte GID = "A"; // Identifier for this specific gun
const byte TRIGGER = 12; // digital input for trigger signal
const byte RATE = 8; // repeat rate in Hz

const char *version="Gallery_gun -> V0.1beta-20150717";

// the setup routine runs once when you press reset:
void setup() {
  pinMode(STATUS_LED, OUTPUT);
  pinMode(TRIGGER, INPUT_PULLUP);
  pinMode(IR_LED, OUTPUT);
  
  digitalWrite(LED, LOW); // make sure the LED is off
  watchdogSetup();
  // initialize serial communication at 115220 bits per second for debugging
  Serial.begin(115200);
  Serial.print(version); Serial.print(" - SN#"); Serial.println(drm_serialno());  
}

byte trig_state = HIGH;
unsigned long start;
// put your main code here, to run repeatedly:
void loop() {
 
  if((trig_state = digitalRead(TRIGGER))==LOW)
  {
    start = millis();
    digitalWrite(LED, HIGH); // light the LED
    
    digitalWrite(LED, LOW);
  }
  
}
