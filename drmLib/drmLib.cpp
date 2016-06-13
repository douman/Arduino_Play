/*
  drmLib.cpp - Utility Library for drm
  Created by drm 20151213
  History
  -------
  V2.0 --> trying to add RTC routines to this library, did not work (see comments)
  V2.1 --> ifdef(ing) for M0 cases
*/

#include "drmLib.h"

// return the the byte BCD encoded value as a int
int drmBcd2Dec(int inbyte) 
{
  return (((inbyte & 0b11110000)>>4)*10 + (inbyte & 0b00001111));
}

// return the serial number from Arduino EEPROM (drm specific)
unsigned short drmSerialNo() 
{
#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) // This is the 128 bit serial number for M0, XOR the bits into a short
  unsigned short serial_xor=0;
  int i;
  unsigned short *serial_no= (unsigned short *) 0x0080A00C;
  for(i=0; i<8; i++) serial_xor ^= *serial_no++;
  return(serial_xor); // no EEPROM on M0 MCU
#else
  return(EEPROM.read(5) << 8 | EEPROM.read(6)); // combine two bytes into in serial number (drm specific)
#endif
}

// Printout the standard drm Arduino start message
void drmStartPrint(const char *drmversion) 
{
  Serial.print(drmversion); 
  Serial.print(F(" SN#"));
  Serial.println(drmSerialNo());
  Serial.print(F("Compiled-> "));
  Serial.print(F(__DATE__)); 
  Serial.print(F(" "));
  Serial.println(F(__TIME__));
}

// Print a long integer with leading zero and a specific number of places (up to 9 digits)
void drmPrtLead0(long in, int places) 
{
  if(places>10 || places<2) return;
  in = abs(in); // only for positive numbers
  if(in < 1000000000) in = in + 1000000000; // extend smaller numbers
  char out_str[11];
  sprintf(out_str, "%ld", in);
  Serial.print((out_str+(10-places)));
}

// Print out a formatted elapsed time given the millisecond timer value
void printTime(unsigned long diff)
{
  const long ms_d =86400000; 
  const long ms_hr = 3600000;
  const long ms_min = 60000;
  const long ms_sec = 1000;
  Serial.print(diff/ms_d); Serial.print("- ");
  diff = diff - ms_d*(diff/ms_d);
  if(diff/ms_hr<10) Serial.print("0");
  Serial.print(diff/ms_hr); Serial.print(":");
  diff = diff - ms_hr*(diff/ms_hr);
  if(diff/ms_min<10) Serial.print("0");
  Serial.print(diff/ms_min); Serial.print(":");
  diff = diff - ms_min*(diff/ms_min);
  if(diff/ms_sec<10) Serial.print("0");
  Serial.print(diff/ms_sec); Serial.print(" \t");
}

/* Save the RTC stuff for later
// Initialize RTC module for reading
int initRTC(rtc_type type)
{
	if(type != DS3231)
	{
		// Unknown type of RTC
		return(ER_RTCUNK);			//init DS3231
	}
	Wire.begin();

  // Write registers to enable (code lifted from ClockDunio)
  // clear /EOSC bit
    Wire.beginTransmission(DS3231_addr);
    Wire.write(0x0E); // start at the CSR
    Wire.write(0b00000110); // write register bitmap, bit 7 is /EOSC
                            // bit 2-0 are interupt/square wave/alarm enables, 
                            // #2 choses between sqwv and alarms
                            // #1 turns on alarm 2
                            // #0 turns on alarm 1
    Wire.write(0b00000000); // Clear the flags in the Status register
    Wire.endTransmission();

// Set up the Alarms
    Wire.beginTransmission(DS3231_addr); // address DS3231
    Wire.write(0x07); // select register -- Alarm registers
    Wire.write(0b10000000);
    Wire.write(0b10000000);
    Wire.write(0b10000000);
    Wire.write(0b10000000);
    Wire.write(0b10000000);
    Wire.write(0b10000000);
    Wire.write(0b10000000); // Alarm 1 every second Alarm 2 every minute
    Wire.endTransmission();

// Set up the Aging offset
    Wire.beginTransmission(DS3231_addr); // address DS3231
    Wire.write(0x10); // Aging offset select register -- Alarm registers
    Wire.write(0b00000000); // No Aging offset
    Wire.endTransmission();

	DS3231_setup();
	return(ST_RTCOK);
}

// Read the RTC contents to get Date, Time & Temperature
struct parseTime readRTC(rtc_type type)
{
	byte readBytes[NUMRTCREGS];

	if(type != DS3231)
	{
		// Unknown type of RTC
		return(ER_RTCUNK);
	}
	// read DS3231
	readClock(readBytes);
	return(decodeTime(readBytes));
}

void readClock(byte *readBytes) 
{
    int i;
    Wire.beginTransmission(DS3231_addr); // DS3231_addr is DS3231 device address
    Wire.write((byte)0); // start at register 0
    Wire.endTransmission();
    Wire.requestFrom(DS3231_addr, num_regs); // request read of all registers
    for (i=0;(i<num_regs && Wire.available());i++) { read_by[i] = Wire.read(); }
    if (i!=num_regs) 
    { 
        Serial.print(F("Read only ")); 
		Serial.print(i); 
		Serial.println(F(" bytes"));
	}
}

// make sense out of the register valuse and 
// put them in the global time structure time_struct
struct parseTime decode_Time(byte *read_by) 
{
	struct parseTime time_struct;
    // The below are all BCD encoded with some high control bits on some
    time_struct->seconds = read_by[0]; // get seconds
    time_struct->minutes = read_by[1]; // get minutes
    time_struct->hours = read_by[2];   // get hours
    time_struct->dow = read_by[3];   // get day of week (Mon = 0)
    time_struct->dom = read_by[4];   // get day of month
    time_struct->month = read_by[5];   // get month number (Jan = 1)
    time_struct->year = read_by[6];   // get year (last two digits)
    time_struct->csr = read_by[14];
    time_struct->sr = read_by[15];
    time_struct->seconds = drmBcd2Dec(time_struct->seconds);
    time_struct->minutes = drmBcd2Dec(time_struct->minutes);
    time_struct->hours = drmBcd2Dec(0x3F & time_struct->hours);
    time_struct->dow = drmBcd2Dec(time_struct->dow);
    time_struct->dom = drmBcd2Dec(time_struct->dom);
    time_struct->month = drmBcd2Dec(0x1F & time_struct->month);
    time_struct->year = drmBcd2Dec(time_struct->year);
    time_struct->int_year = 2000 + (100*((int) time_struct->month>32)) + 
							(int) time_struct->year;
    time_struct->lsec = time_struct->seconds + 60*(time_struct->minutes + 
						60*(time_struct->hours + 24*time_struct->dom));
	time_struct->tempf = 3200 + ((read_by[17]*100 + (read_by[18] >> 6)*25)*9)/5;
	return(time_struct);
}
*/

