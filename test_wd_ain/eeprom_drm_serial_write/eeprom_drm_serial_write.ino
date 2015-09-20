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
 */

#include <EEPROM.h>

const char *version="eeprom_drm_serial_write -> V1.0.3-20141225";

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int high_byte = 5;
int low_byte = 6;
// Serial number to be written
int serial = 10010;

void setup()
{
  Serial.begin(115200);
  drm_Start_print();
}

void loop()
{
  Serial.print(" - SN#");
  Serial.println(drm_Serialno());
  if(EEPROM.read(high_byte) != (serial >> 8)) EEPROM.write(high_byte, serial >> 8);
  if(EEPROM.read(low_byte) != (serial & 255)) EEPROM.write(low_byte, serial & 255);
  for (int i=low_byte+1; i<100; i++) if(EEPROM.read(i) != i) EEPROM.write(i, i);
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
