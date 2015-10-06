/*
  Analog input, analog output, serial output
  Arduino example modified by drm 20150921 for PWM LED modulation

 uses value to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.

 The circuit:
 * PN2222 base connected to digital pin 3 through 100 Ohm resistor

 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe

 */
#include <EEPROM.h>
const char *version="PWM_Test_LED -> V1.0-20150921 ";

// These constants won't change.  They're used to give names
// to the pins used:
const int ANALOGOUTPIN = 3; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
}

void loop() {
  // Set the intensity value by looping
  for(sensorValue = 2; sensorValue<1025; sensorValue+=sensorValue)
  {
    // map it to the range of the analog out:
    outputValue = map(sensorValue, 0, 1023, 0, 255);
    // change the analog out value:
    analogWrite(ANALOGOUTPIN, outputValue);
  
    // print the results to the serial monitor:
    Serial.print("sensor = " );
    Serial.print(sensorValue);
    Serial.print("\t output = ");
    Serial.println(outputValue);
  
    // wait 20 milliseconds before the next loop
    // for the analog-to-digital converter to settle
    // after the last reading:
    delay(500);
  }
  delay(1500);
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
