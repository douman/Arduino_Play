/*
  diveLog.cpp - Support/Utility Library for dive logger
  Created by drm 20160617
  History
  -------
  V0.1 --> first pass
*/

#include "diveLog.h"

int wr_log(ser, char* txtbuf)
{
	ser.print(log_num++); ser.print("\t");
	ser.println(*txtbuf);
	return(ST_AOK);
}

int bld_time(int y, int m, int d, int h, int mi, int s, int milli, char* timestr)
{

}

int fmt_rtc(char* timestr, char* txtbuf)
{
}

int fmt_comm(char* comm, char* txtbuf)
{
}

int fmt_encry(char* encry, char* txtbuf)
{
}

int fmt_accl(float ax, float ay, float az, char* txtbuf)
{
}

int fmt_gyr(float gx, float gy, float gz, char* txtbuf)
{
}

int fmt_mag(float mx, float my, float mz, char* txtbuf)
{
}

int fmt_gps(int ltdeg, float ltmin, int lndeg, float lnmin, float alt, float vx, float vy, float vz, float gpserr, char* timestr, char* txtbuf)
{
}

int fmt_pres(float pres,  char* txtbuf)
{
}

int fmt_temp(float temp,  char* txtbuf)
{
}

int fmt_lux(float lux,  char* txtbuf)
{
}

int fmt_acoustic(float acoustic,  char* txtbuf)
{
}


