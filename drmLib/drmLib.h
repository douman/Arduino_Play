/*
  drmLib.h - Utility Library for drm 
  To use put #include <drmLib.h> in your source code
  Created by drm 20151213
  V1.0 --> First with EEPROM access and start print
  V2.0 --> adding RTC access
  V2.1 --> ifdef(ing) for M0 cases
*/
#ifndef drmLib_h
#define drmLib_h
#define drmLibVer "drmLib V2.1 -- 20160603"


#if !defined(ARDUINO_ARCH_SAM) && !defined(ARDUINO_ARCH_SAMD) && !defined(ESP8266) && !defined(ARDUINO_ARCH_STM32F2)
  #include <avr/sleep.h> // AVR sleep modes (power saving)
  #include <avr/power.h> // AVR power states
  #include <avr/wdt.h>   // AVR watch dog timer
  #include <util/delay.h>
  #include <EEPROM.h>    // AVR on chip EEPROM 
#endif


#include "Arduino.h"
#include <Wire.h>      // I2C and SPI
#include <SPI.h>
#include <TM1637Display.h> // 4 digit 7 seg display

// Doug's Data structures, constants and enums
#define ER_BADID -20 // bad ID on requested operation
#define ER_BADOPEN -21 // error opening file
#define ER_UNEXPFIO -22 // unexpected result with file IO
#define ST_AOK 0	// Everything is good
#define ST_NOERR 1	// No error occured
#define ST_NOOP 2	// Nothing happened
#define ER_ERR -1 // generic error
#define ER_UNK -1001 // unknown error 
#define FALSE 1!=1
#define TRUE 1==1

#define MAX_LINE 180    // Max line buffer length
#define NUMRTCREGS 19   // numbmer of registers on the RTC

/* Can't do the struct thing in Arduino for some reason
 *
enum rtc_type 
{
	DS3231,
	OTHER
}
 */

typedef struct parseTime {
  unsigned long lsec;
  long tempf; // F temperature * 100 (poor mans float)
  int int_year;
  byte seconds;
  byte minutes;
  byte hours;
  byte dow;
  byte dom;
  byte month;
  byte year;
  byte csr;
  byte sr;
};
struct parseTime time_struct[1];

// Routines in this Library
int drmBcd2Dec(int inbyte);
unsigned short drmSerialNo();
void drmStartPrint(const char *drmversion);
void drmPrtLead0(long in, int places);
void printTime(unsigned long milli_time);

/* Save the RTC for later
int initRTC(rtc_type type);
struct parseTime readRTC(rtc_type type);
void readClock(byte *readBytes);
struct parseTime decodeTime(byte *readBytes);
*/

#endif
