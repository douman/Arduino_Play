/*
  diveLog.h - Utility Library for drm 
  To use put #include <diveLog.h> in your source code
  Created by drm 20151213
  V1.0 --> First with EEPROM access and start print
  V2.0 --> adding RTC access
  V2.1 --> ifdef(ing) for M0 cases
*/

#include <drmLib.h>

#ifndef diveLog_h
#define diveLog_h
#define diveLogLibVer "diveLog V0.1 -- 20160617"

#if !defined(ARDUINO_ARCH_SAM) && !defined(ARDUINO_ARCH_SAMD) && !defined(ESP8266) && !defined(ARDUINO_ARCH_STM32F2)
// Here is if we are other than SAM
#else
// Here is if we are SAM
#endif
// Global for Dive Logger

unsigned long log_num=0;

// Routines in this Library
// Should really make the Log an object interface but for now
//
int wr_log(ser, char* txtbuf);
int bld_time(int y, int m, int d, int h, int mi, int s, int milli, char* timestr);
int fmt_rtc(char* timestr, char* txtbuf);
int fmt_comm(char* comm, char* txtbuf);
int fmt_encry(char* encry, char* txtbuf);
int fmt_accl(float ax, float ay, float az, char* txtbuf);
int fmt_gyr(float gx, float gy, float gz, char* txtbuf);
int fmt_mag(float mx, float my, float mz, char* txtbuf);
int fmt_gps(int ltdeg, float ltmin, int lndeg, float lnmin, float alt, float vx, float vy, float vz, float gpserr, char* timestr, char* txtbuf);
int fmt_pres(float pres,  char* txtbuf);
int fmt_temp(float temp,  char* txtbuf);
int fmt_lux(float lux,  char* txtbuf);
int fmt_acoustic(float acoustic,  char* txtbuf);

#endif
