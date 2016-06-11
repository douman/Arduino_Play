#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
#include <drmLib.h>
#include <Adafruit_GPS.h>
#include <Adafruit_BluefruitLE_UART.h>

const char *version="M0_Feather_GPS -> V0.9-20160610 ";

// #include <Adafruit_BLE.h>
// #include <Adafruit_BluefruitLE_SPI.h>
// #include <SPI.h>

/* Utilitize the GPS with M0 Feather BLE
 *  with liberal code borrowing from Adafruit
 *  
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
   
  // initialize digital pin (LED) 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(BATT, INPUT); // Battery level adc input
  
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
  attachInterrupt(digitalPinToInterrupt(GPSPPSINT), pps_int, RISING);
}

// the loop function runs over and over again forever
void loop() 
{
  interrupts(); // Make sure interrupts are on
  // Serial.println(analogRead(BATT));
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(0);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(0);              // wait for a second
  // Serial.println("Reg Serial.println");
  // SerialUSB.println("USB Serial.println");

  if (Serial.available()) 
  {
    char c = Serial.read();
    Serial1.write(c);
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
        Serial.print(sentence[4]);
        if (myGPS.fix && sentence[4] == 'R') 
        {
          Serial.println();
          Serial.print(myGPS.month, DEC); Serial.print('/');
          Serial.print(myGPS.day, DEC); Serial.print("/20");
          Serial.print(myGPS.year, DEC);
          Serial.print("- ");
          Serial.print(myGPS.hour, DEC); Serial.print(':');
          Serial.print(myGPS.minute, DEC); Serial.print(':');
          Serial.print(myGPS.seconds, DEC); Serial.print('.');
          Serial.print(myGPS.milliseconds); Serial.print("(");
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

          Serial.print(micro_intv); Serial.print(" ");
          Serial.println(icnt);
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
