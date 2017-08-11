#define ONTIME 100
#define OFFTIME 500
const char *code_version="Blink_exp_Mod_20140523 -> V1.0-20170803";
#include <drmLib.h>
/*
  Blink
 Turns on an LED on and off, repeatedly
  */
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int j;
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  Serial.begin(115200);
  pinMode(led, OUTPUT);     
  j=0;
  drmStartPrint(code_version);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("On");
  delay(ONTIME);              // wait a bit
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  Serial.println("Off");
  delay(OFFTIME);              // wait a bit more
}

