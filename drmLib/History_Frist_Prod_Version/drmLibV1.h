/*
  drmLib.h - Utility Library for drm 
  Created by drm 20151213
*/
#ifndef drmLib_h
#define drmLib_h

#include "Arduino.h"
#include <EEPROM.h>

byte drmBcd2Dec(byte inbyte);
unsigned short drmSerialNo();
void drmStartPrint(const char *drmversion);
void drmPrtLead0(long in, int places);
void printTime(unsigned long milli_time);

#endif
