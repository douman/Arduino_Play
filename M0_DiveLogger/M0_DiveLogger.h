#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
#include <drmLib.h>
#include <Adafruit_GPS.h>
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BluefruitLE_UART.h>

#include "BluefruitConfig.h"

#define OUT_SIZE 160
#define GPS_BAUD 9600 // 9600 is factory, 57600 is faster
#define BAT_AVG_CNT 4
#define BLEMOD 20
#define GPSmillis 1000
#define NINEDoFmillis 1000
// #define DEBUG

// Define pins for M0 Feather BLE DiverLogger
#define LED           13
#define BATT          A7
#define GPSPPSINT     11
#define ACCINT        8
#define MICIN         A10
#define GPSRX         16
#define GPSTX         15
#define NEOOUT        12
#define GPSENABLE     7
#define LOGRX         6
#define LOGTX         5
#define SCL           21
#define SDA           20

// Globals for the M0_Feather_GPS sketch

long log_cnt=0;
unsigned long lastGPSmillis = 0, lastNINEDoFmillis = 0, serprt_millis = 0;
byte cksum, savecksum;
volatile unsigned long micro_beg=0, micro_end=0, micro_intv=999, ppscnt=0; // gps timing globals
float micro_factor=1.000, micro_corr = 0;
float batt_volts;
volatile unsigned long rtc_sec_cnt=0, rtc_32768_cnt=0, rtc_latest_micro; // rtc timing globals
boolean bleprt = false, serprt=true, wrt_ble = false;
volatile boolean new_sec = false;

