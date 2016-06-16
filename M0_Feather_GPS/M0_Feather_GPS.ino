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

const char *version="M0_Feather_GPS -> V0.93-20160613 ";

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

/* Utilitize the GPS with M0 Feather BLE
 *  with liberal code borrowing from Adafruit
 *  
 *  V0.9 by drm 20160609
 *  V0.91 by drm 20160613 added time adjustment
 *  V0.92 by drm 20160613 got the factors for micro timing dialed in (I think)
 */
#define GPS_BAUD 9600 // 9600 is factory, 57600 is faster
#define BAT_AVG_CNT 4
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

Adafruit_GPS myGPS(&Serial1);                  // Ultimate GPS FeatherWing
Adafruit_LSM9DS0 my9DOF = Adafruit_LSM9DS0();  // i2c 9DOF sensor
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); // Bluetooth LE

byte cksum, savecksum;
volatile unsigned long micro_beg=0, micro_end=0, micro_intv=999, icnt=0;
float micro_factor=1.000, micro_corr = 0;
volatile boolean new_sec = false;
// the setup function runs once when you press reset or power the board

void pps_int()
{
  micro_end = micros();
  micro_intv = micro_end - micro_beg;
  micro_beg = micro_end;
  new_sec = true;
  icnt++;
}

// A small helper -- ADAFRUIT
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}
void setup() 
{
  Serial.begin(115200);
  drmStartPrint(version);
   
  // initialize digital pin (LED) 13 as an output and the battery level as analog input
  pinMode(LED, OUTPUT); digitalWrite(LED, LOW);
  pinMode(BATT, INPUT); // Battery level adc input
  
  // Set up the Bluetooth LE
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("BLE OK!"));
    if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() )
    {
      error(F("Couldn't factory reset"));
    }
  }
  ble.verbose(false);  // debug info is a little annoying after this point!
  /* Disable command echo from Bluefruit */
  ble.echo(false);
  // Set module to DATA mode
  ble.setMode(BLUEFRUIT_MODE_DATA);


  // Set up the GPS
  pinMode(GPSPPSINT, INPUT);
  pinMode (GPSENABLE, OUTPUT);
  digitalWrite(GPSENABLE, LOW); // enable the GPS
  Serial1.begin(GPS_BAUD);
  myGPS.begin(GPS_BAUD);

  // Set up the 9DOF shield
  // my9DOF.begin();
/*    if(!my9DOF.begin())
  {
    // There was a problem detecting the LSM9DS0 ... check your connections
    Serial.print(F("Ooops, no LSM9DS0 detected ... Check your wiring - looping!"));
    while(1);
  }


  // 1.) Set the accelerometer range
  my9DOF.setupAccel(my9DOF.LSM9DS0_ACCELRANGE_2G);  
  // 2.) Set the magnetometer sensitivity
  my9DOF.setupMag(my9DOF.LSM9DS0_MAGGAIN_2GAUSS);
  // 3.) Setup the gyroscope
  my9DOF.setupGyro(my9DOF.LSM9DS0_GYROSCALE_245DPS);
*/

  // set up the GPS PPS interupt driver
  attachInterrupt(digitalPinToInterrupt(GPSPPSINT), pps_int, FALLING);
}

// the loop function runs over and over again forever
void loop() 
{
  interrupts(); // Make sure interrupts are on

  // send any serial input straight to the GPS unit
  if (Serial.available()) 
  {
    char c = Serial.read();
    Serial1.write(c);
  }
  if(new_sec && icnt > 10)
  {
    long delta = 1000000 - micro_intv;
    if(icnt == 11) micro_corr = delta;
    micro_corr = (99 * micro_corr + delta)/100.0;
    new_sec = false;
  }
  if (Serial1.available()) 
  {
    char c = myGPS.read();
    
    if (c=='$')
    {
      // Get and print Battery voltage at beginning of GPS sentence
      float val=0;
      int i;
      for(i=0; i<BAT_AVG_CNT; i++) val += analogRead(BATT);
      val = val/(float)BAT_AVG_CNT;
      val = (val * (2*3.3))/1024;
#ifdef DEBUG
      Serial.print("Chsum-> ");
      Serial.print(savecksum, HEX);
#endif

      // Print the parsed values
      if (myGPS.newNMEAreceived()) 
      {
        myGPS.parse(myGPS.lastNMEA());
#ifdef DEBUG
        Serial.print(" - Fx: "); Serial.print((int)myGPS.fix);
        Serial.print(" quality: "); Serial.println((int)myGPS.fixquality);
#endif
        char *sentence = myGPS.lastNMEA();
        Serial.print(sentence[4]); // uniquely identify what kind of NMEA sentance
        if (myGPS.fix && sentence[4] == 'R') // print only for "R" and we have a fix
        {
          digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
          if(millis() < 10000) 
          {
            drmStartPrint(version);
            Serial.print(micro_intv); Serial.print(" ");
            Serial.println(icnt);
            print_serial();
          }
          Serial.println();
          drmPrtLead0(myGPS.month, 2); Serial.print('/');
          drmPrtLead0(myGPS.day, 2);; Serial.print("/20");
          drmPrtLead0(myGPS.year, 2);
          Serial.print("- ");
          drmPrtLead0(myGPS.hour, 2); Serial.print(':');
          drmPrtLead0(myGPS.minute, 2); Serial.print(':');
          drmPrtLead0(myGPS.seconds, 2); Serial.print('.');
          drmPrtLead0(myGPS.milliseconds, 2); Serial.print("(");
          Serial.print(micro_intv);
          Serial.print(") B-> ");
          Serial.print(val, 3);
          Serial.print("V -> ");
          int temp = (int) myGPS.latitude/100;
          Serial.print(temp); Serial.print(" ");
          Serial.print(myGPS.latitude - 100 * temp, 4); Serial.print(" "); Serial.print(myGPS.lat);
          Serial.print(", ");
          temp = (int) myGPS.longitude/100;
          Serial.print(temp); Serial.print(" ");
          Serial.print(myGPS.longitude - 100 * temp, 4); Serial.print(" "); Serial.print(myGPS.lon);
          Serial.print(" "); Serial.print(myGPS.speed);
          Serial.print("Kt "); Serial.print(myGPS.altitude);
          Serial.print("M S- "); Serial.println((int)myGPS.satellites);

          Serial.print(micro_intv); Serial.print(" ");
          Serial.print(((float)micro_intv) + micro_corr,1); Serial.print(" ");
          Serial.print(micro_corr, 2); Serial.print(" ");
          Serial.println(icnt);
          digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW

          if(ble.isConnected())
          {
                ble.println(val,3);
                Serial.println("finished ble write");
          }
        }
      }
      
      // Start for Calculating sentence checksum
      cksum = 0;
    }
    else if (c!='*')
    {
      cksum = cksum ^ c; // xor for checksum calculation
    }
    else
    {
      savecksum = cksum;
    }
    // Serial.write(c);
  }
}
void print_serial()
{
  long s[4], *ser;
  ser = (long *) 0x0080A00C;
  int i;
  for (i=3; i>=0; i--) s[i] = *ser++;

  Serial.print("Serial-> ");
  for(i=0; i<4; i++) Serial.print(s[i], HEX);
  Serial.println();
}
