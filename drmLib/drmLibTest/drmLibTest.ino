/*
 * drmLibTest -- Test routines in the drmLib Arduino library
 * V1.0 by drm on 20160207
 * 
 */

#include <EEPROM.h>

const char *version="drmLibTest -> V1.0-20160207";

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
const int SERIAL_HIGH_BYTE = 5;
const int SERIAL_LOW_BYTE = 6;
// Serial number to be written
int serial = 10002;

void setup()
{
  drm_Start_print();
}

void loop()
{
  drm_Start_print();
  delay(2000);
}
