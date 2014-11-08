/*
 * eeprom_drm_serial_write
 *
 * Stores a "serial number" in bytes 5 (high order byte) and
 * 6 (low order byte) 
 *
 * drm sketches print out this value when they start
 * drm 20141024
 */

#include <EEPROM.h>

const char *version="eeprom_drm_serial_write -> V1.0.0-20141024";

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int high_byte = 5;
int low_byte = 6;
// 10002 is RedBoard
// 10003 is 1st 5v mini Pro
//
int serial = 10002;

void setup()
{
  Serial.begin(115200);
  Serial.print(version); Serial.print(" - SN#");
  Serial.println(drm_serialno());
}

void loop()
{
  Serial.print(" - SN#");
  Serial.println(drm_serialno());
  if(EEPROM.read(high_byte) != (serial >> 8)) EEPROM.write(high_byte, serial >> 8);
  if(EEPROM.read(low_byte) != (serial & 255)) EEPROM.write(low_byte, serial & 255);
  delay(2000);
}

short drm_serialno() {
  return(EEPROM.read(5) << 8 | EEPROM.read(6));
}
