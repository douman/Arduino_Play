/*
 * eeprom_drm_serial_write
 *
 * Stores a "serial number" in bytes 5 (high order byte) and
 * 6 (low order byte) 
 *
 * drm sketches print out this value when they start
 * drm 20141024
 * 10001 is 1st Uno R3
 * 10002 is RedBoard
 * 10003 is 1st 5v mini Pro
 * 10004 is 1st 3.3v mini Pro (sparkfun)
 * 10005 is 3.3v full size Pro (sparkfun)
 * 10006 is 5v china mini Pro 
 * 10007 is 5v china mini Pro with all headers
 * 10008 is 5v china mini Pro
 * 10009 is 3.3v mini Pro (sparkfun)
 * 10010 is 5v uno clone from china 20150915
 * 10011 is 3.3 china Pro mini (backwards serial connector)
 * 
 * RTC Serial Numbers
 * 20000 is RTC000 
 * 20001 is RTC001 installed with Pi 2 in bedroom
 * 20002 is RTC002 installed with weather station
 * 20003 is RTC003 installed with DHT & TMP36 prototype
 * 
 */

#include <EEPROM.h>

const char *version="eeprom_drm_serial_write -> V1.0.4-20151010";

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
const int SERIAL_HIGH_BYTE = 5;
const int SERIAL_LOW_BYTE = 6;
// Serial number to be written
int serial = 10002;

void setup()
{
  Serial.begin(19200);
  drm_Start_print();
}

void loop()
{
  byte read_char;
  
  Serial.print(" - SN#");
  Serial.println(drm_Serialno());
  if(Serial.available()) read_char = Serial.read();
  if(read_char == 'w')
  {
    if(EEPROM.read(SERIAL_HIGH_BYTE) != (serial >> 8)) EEPROM.write(SERIAL_HIGH_BYTE, serial >> 8);
    if(EEPROM.read(SERIAL_LOW_BYTE) != (serial & 255)) EEPROM.write(SERIAL_LOW_BYTE, serial & 255);
    for (int i=SERIAL_LOW_BYTE+1; i<100; i++) if(EEPROM.read(i) != i) EEPROM.write(i, i);
    read_char = NULL;
  }
  delay(2000);
}


void drm_Start_print() {
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
