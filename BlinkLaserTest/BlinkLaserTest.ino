/* Blink without Delay
 Turns on and off a light emitting diode (LED) connected to a digital
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.

 The circuit:
 * LED attached from pin 13 to ground.
 * Note: on most Arduinos, there is already an LED on the board
 that's attached to pin 13, so no hardware is needed for this example.
 
 derived by drm 20150823
 From example created 2005 by
 by David A. Mellis
 modified 8 Feb 2010
 by Paul Stoffregen
 modified 11 Nov 2013
 by Scott Fitzgerald
  
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */
#include <Arduino.h>
#include <EEPROM.h>

#define ledPin  13     // the number of the LED pin
#define lazPin  8      // the number of the LED pin
#define trig_1  2      // first trigger wire/driven (enable)
#define trig_2  3      // second trigger wire/read (trigger read)

const char *version="BlinkLaserTest_20150823 -> V1.0-20150823 ";

// Variables will change :
int lState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 50;           // interval at which to blink (milliseconds)
int trigger=HIGH;
boolean trig_oneshot=false;

void setup() {
  Serial.begin(115200);
  drm_Start_print();

  // set the digital pin as output:
  pinMode(ledPin, OUTPUT); digitalWrite(ledPin, LOW);  
  pinMode(lazPin, OUTPUT); digitalWrite(lazPin, LOW);
  pinMode(trig_1, OUTPUT); digitalWrite(trig_1, LOW);
  pinMode(trig_2, INPUT_PULLUP);
}

void loop()
{
  // check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time you blinked
  // the LED is bigger than the interval at which you want to
  // blink the LED.
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    trigger = digitalRead(trig_2);

    // if the LED is off turn it on and vice-versa:
    if (lState == LOW && trigger == LOW)
    {
      lState = HIGH;
      if(trig_oneshot == false)
      {
        Serial.println("Triggered!");
        trig_oneshot = true;
      }
    }
    else
    {
      lState = LOW;
      if(trigger == HIGH) trig_oneshot = false;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, lState);
    digitalWrite(lazPin, lState);  }
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
