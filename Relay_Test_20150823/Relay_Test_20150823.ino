/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
#include <Arduino.h>
#include <EEPROM.h>

#define PIN 7 // pin connected to relay
#define LED 13

const char *version="Relay_Test_20150823 -> V1.0-20150823 ";

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  drm_Start_print();

  // initialize digital pin 13 as an output.
  pinMode(LED, OUTPUT);
  pinMode(PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(PIN, HIGH);   // turn the Relay on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(PIN, LOW);    // turn the Relay off by making the voltage LOW
  delay(1000);              // wait for a second
}
void drm_Start_print() 
{
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
