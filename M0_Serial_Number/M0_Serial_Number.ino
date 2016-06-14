#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
#include <drmLib.h>
#include <Adafruit_GPS.h>
#include <Adafruit_BluefruitLE_UART.h>

const char *version="M0_Feather_GPS -> V0.9-20160610 ";

// #include <Adafruit_BLE.h>
// #include <Adafruit_BluefruitLE_SPI.h>
// #include <SPI.h>

/*  Read the Atmel SAMD21G18 Serial number registers and print them\
 *  V0.9 by drm 20160609
 */
#define GPS_BAUD 9600 // 9600 is factory, 57600 is faster
#define BAT_AVG_CNT 4
// #define DEBUG

// Define pins for M0 Feather BLE DiverLogger
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
byte cksum, savecksum;
volatile unsigned long micro_beg=0, micro_end=0, micro_intv=999, icnt=0;
// the setup function runs once when you press reset or power the board

void pps_int()
{
  micro_end = micros();
  micro_intv = micro_end - micro_beg;
  micro_beg = micro_end;
  icnt++;
}
void setup() 
{
  Serial.begin(115200);
  drmStartPrint(version);
   
  // initialize digital pin (LED) 13 as an output and the battery level as analog input
  pinMode(13, OUTPUT);
  pinMode(BATT, INPUT); // Battery level adc input
  
  // Set up the GPS
  pinMode(GPSPPSINT, INPUT);
  pinMode (GPSENABLE, OUTPUT);
  digitalWrite(GPSENABLE, LOW); // enable the GPS
  Serial1.begin(GPS_BAUD);
  myGPS.begin(GPS_BAUD);
}

// the loop function runs over and over again forever
void loop() 
{
  // send any serial input straight to the GPS unit
  if (Serial.available()) 
  {
    char c = Serial.read();
    Serial1.write(c);
  }
  if (Serial1.available()) 
  {
    char c = myGPS.read();
    
    if (c=='$') // Beginning of a GPS sentence
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
          Serial.print(myGPS.latitude, 4); Serial.print("-"); Serial.print(myGPS.lat);
          Serial.print(", ");
          Serial.print(myGPS.longitude, 4); Serial.print("-"); Serial.print(myGPS.lon);
          Serial.print(" "); Serial.print(myGPS.speed);
          Serial.print("Kt "); Serial.print(myGPS.altitude);
          Serial.print("M S- "); Serial.println((int)myGPS.satellites);
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

