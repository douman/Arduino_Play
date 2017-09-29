/* Test the MAX6675 Thermocouple board and K-type thermocouple
 * derived by drm 20150823
 */
const char *VERSION="MAX6675_Test -> V1.0-20170927 ";
const long BAUD = 115200;

// derived from
// www.ladyada.net/learn/sensors/thermocouple

#include <max6675.h>
#include <Wire.h>
#include <drmLib.h>

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  Serial.begin(BAUD);
  drmStartPrint(VERSION);
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp
  Serial.print(thermocouple.readFahrenheit());  
  Serial.print("/");
  Serial.println(thermocouple.readCelsius());
  delay(1000);
}
