/* AlarmSystem 
  by drm starting 20170701 
  V0.1 just fleshing out the skeleton
 */
#include <Arduino.h>
#include <EEPROM.h>
#include <drmLib.h>
#include "AlarmSystem.h"

const char *version="AlarmSystem -> V0.1-20170701 ";

// Variables will change :
int lState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

void setup() {
  Serial.begin(115200);
  drm_Start_print();

  // set the digital pin as output:
  pinMode(ledPin, OUTPUT); digitalWrite(ledPin, LOW);  
  pinMode(lazPin, OUTPUT); digitalWrite(lazPin, LOW);
  pinMode(trig_1, OUTPUT); digitalWrite(trig_1, LOW);
  pinMode(trig_2, INPUT_PULLUP);

  // sensor state initialization
  
  // alarm initialization
  alarm_state = FALSE;
  yellow_flash = FALSE;
  red_flash = FALSE;
  laser_flash = FALSE;
  horn_blast = FALSE;
  start_time = millis();
  
}

unsigned long elapSec(unsigned long from_millis)
{
  return((millis() - from_millis)/1000)
}

void loop()
{
  // Check the Sensors
  
}
