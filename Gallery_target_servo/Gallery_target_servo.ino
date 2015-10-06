/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2

stripped back to servos only drm 20150908

For use with the Adafruit Motor Shield v2.3 
---->	http://www.adafruit.com/products/1438

Connect hobby servo to SERVO1 (yellow wire to outsid, I think)
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h>
#include <EEPROM.h>

const char *version="Gallery_target_servo -> V1.0-20150925 ";

// Create the motor shield object with the default I2C address
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// We'll also test out the built in Arduino Servo library
Servo servo1;

void setup() 
{
  Serial.begin(115200);           // set up Serial library at 9600 bps
  drm_Start_print();
  Serial.println("Maker Faire Target Servo Control");

  // Attach a servo to pin #10
  servo1.attach(10);
}

int i;
void loop() {
  for (i=0; i<180; i+=32)
  {
    servo1.write(min(i,180));
    Serial.print(i);
    Serial.print(" ");
    delay(4);
  }
  delay(1000);
  servo1.write(0);
  Serial.print("Back to 0");
  delay(2000);
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
