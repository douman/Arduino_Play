/* Initial Stepper Motor code for use with Adafruit MotorshieldV2.3

by drm 20150904
 From code examples on adafruit website
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <EEPROM.h>

#define MOT_PORT 1
#define STEPS_REV 200

const char *version = "Stepper_Test_20150904 -> V10.9-20150904 ";

Adafruit_StepperMotor *myMotor = AFMS.getStepper(STEPS_REV, MOT_PORT);

void setup() {
  Serial.begin(115200);
  drm_Start_print();

  // Set up the stepper motor
  myMotor.setSpeed(30); // initial test speed 30 rpm

}

void loop()
{
  for (int i = 0; i < 100; i++)
    myMotor.step(100, FORWARD, MICROSTEP);

  Delay(100);

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
  return (EEPROM.read(5) << 8 | EEPROM.read(6)); // combine two bytes into in serial number (drm specific)
}
