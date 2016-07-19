#include "M0_Feather_GPS.h"
const char *version="M0_Feather_GPS -> V1.4-20160718 ";

// These are for bluefruit
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

/* Utilitize the GPS with M0 Feather BLE
 *  with liberal code borrowing from Adafruit
 *  
 *  V0.9 by drm 20160609
 *  V0.91 by drm 20160613 added time adjustment
 *  V0.92 by drm 20160613 got the factors for micro timing dialed in (I think)
 *  V0.93 by drm 20160616 more work on timing corrections
 *  V0.94 by drm 20160617 now using all 12 bits of ADC
 *  V1.0 by drm 20160621 cleaning up the string output and getting BLE working
 *  V1.1 by drm 20160622 serial toggles for various output (serial/BLE)
 *  V1.2 by drm 20160705 messing with output formats and sprintf for leading zeros
 *  V1.3 by drm 20160706 adjusting messages to sw design document
 *  V1.4 by drm 20160718 adding in 9 DoF sensor and update to 5 hz fix & reporting
 */

Adafruit_GPS myGPS(&Serial1);                  // Ultimate GPS FeatherWing
Adafruit_LSM9DS0 my9DOF = Adafruit_LSM9DS0();  // i2c 9DOF sensor
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); // Bluetooth LE

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
void error(const __FlashStringHelper*err)
{
  if(serprt) Serial.println(err);
  while (1);
}

//    Configures the gain and integration time for the TSL2561
void configure9dof(void)
{
  // 1.) Set the accelerometer range
  my9DOF.setupAccel(my9DOF.LSM9DS0_ACCELRANGE_2G);
  
  // 2.) Set the magnetometer sensitivity
  my9DOF.setupMag(my9DOF.LSM9DS0_MAGGAIN_2GAUSS);

  // 3.) Setup the gyroscope
  my9DOF.setupGyro(my9DOF.LSM9DS0_GYROSCALE_245DPS);
}

void setup() 
{
  Serial.begin(115200);
  drmStartPrint(version);
   
  // initialize digital pin (LED) 13 as an output and the battery level as analog input
  pinMode(LED, OUTPUT); digitalWrite(LED, LOW);
  pinMode(BATT, INPUT); // Battery level adc input
  analogReadResolution(12);

  /* Initialise the 9DOF sensor */
  if(!my9DOF.begin())
  {
    /* There was a problem detecting the LSM9DS0 ... check your connections */
    Serial.print(F("Ooops, no LSM9DS0 detected ... Check your wiring or I2C ADDR!"));
    while(1);
  }
  Serial.println(F("Found LSM9DS0 9DOF"));
  configure9dof();
  
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
  myGPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ); // 5/sec fixes
  myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ); // 5/sec updates
  myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA); // all sentences
  // myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY); // only the R sentence
  
  // set up the GPS PPS interupt driver
  attachInterrupt(digitalPinToInterrupt(GPSPPSINT), pps_int, RISING);
}

// Read the IMU sensor
void read9dof(void)
{
  digitalWrite(LED, HIGH);
  sensors_event_t accel, mag, gyro, temp;
  my9DOF.getEvent(&accel, &mag, &gyro, &temp); 
  digitalWrite(LED, LOW);

  // Write the data
  String out = String(OUT_SIZE);

  out = String(log_cnt++) + "\tacl\t" + String(accel.acceleration.x, 2) + "\t" + 
                                        String(accel.acceleration.y, 2) + "\t" + 
                                        String(accel.acceleration.z, 2) + "\r\n";
  out = out + 
        String(log_cnt++) + "\tgyr\t" + String(gyro.gyro.x, 2) + "\t" + 
                                        String(gyro.gyro.y, 2) + "\t" + 
                                        String(gyro.gyro.z, 2) + "\r\n";
  out = out + 
        String(log_cnt++) + "\tmag\t" + String(mag.magnetic.x, 2) + "\t" + 
                                        String(mag.magnetic.y, 2) + "\t" + 
                                        String(mag.magnetic.z, 2) + "\r\n";
  if(serprt) Serial.println(out);
}

// print out the SAMD processor serial number
void print_serial()
{
  long s[4], *ser;
  ser = (long *) 0x0080A00C; // address of the processor serial number
  int i;
  for (i=3; i>=0; i--) s[i] = *ser++;

  if(serprt) Serial.print("s ");
  for(i=0; i<4; i++) if(serprt) Serial.print(s[i], HEX);
  if(serprt) Serial.println();
}

// the loop function runs over and over again forever
void loop() 
{
  String out = String(OUT_SIZE);
  int out_off=0;
  interrupts(); // Make sure interrupts are on

  // send any serial input straight to the GPS unit
  if (Serial.available()) 
  {
    char c = Serial.read();
    if(c == 'B' || c == 'b') { bleprt = wrt_ble = !wrt_ble; Serial.print("BLE swap "); if(wrt_ble) Serial.print(" TRUE "); Serial.println(wrt_ble); }
    if(c == 'S' || c == 's') { serprt = !serprt; Serial.print("SER swap "); if(serprt) Serial.print(" TRUE "); Serial.println(serprt); }
    Serial1.write(c);
  }
/*  
  if (ble.available()) 
  {
    char c = ble.read();
    if(c == 'B' || c == 'b') { wrt_ble = !wrt_ble; Serial.print("BLE swap "); if(wrt_ble) Serial.print(" TRUE "); Serial.println(wrt_ble); }
    if(c == 'S' || c == 's') { serprt = !serprt; Serial.print("SER swap "); if(serprt) Serial.print(" TRUE "); Serial.println(serprt); }
    Serial1.write(c);
  }
 *   
 */
  // Recalculate the 1sec microsecond correction (only for rational values of measured microseconds)
  if(new_sec && icnt > 10 && micro_intv > 999000 && micro_intv < 1001000)
  {
    long delta = 1000000 - micro_intv;
    if(icnt == 11) micro_corr = delta;
    micro_corr = (7.0 * micro_corr + ((float) delta))/8.0;
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
      // val = (val * (2*3.3))/1024; // at lower ADC resolution
      val = (val * (2*3.3))/4096;
#ifdef DEBUG
      if(serprt) Serial.println(myGPS.newNMEAreceived());
      if(serprt) Serial.print("Chsum-> ");
      if(serprt) Serial.println(savecksum, HEX);
      if(serprt) Serial.print(" - Fx: "); if(serprt) Serial.print((int)myGPS.fix);
      if(serprt) Serial.print(" quality: "); if(serprt) Serial.println((int)myGPS.fixquality);
#endif

      // Print the parsed values
      if (myGPS.newNMEAreceived()) 
      {
        myGPS.parse(myGPS.lastNMEA());
        char *sentence = myGPS.lastNMEA();
        if(serprt) 
        {
          Serial.print(sentence[4]); // uniquely identify what kind of NMEA sentance
          if (sentence[4] == 'R') Serial.print(myGPS.satellites);
        }
        if (myGPS.fix && sentence[4] == 'R') // print only for "R" and we have a fix
        {
          digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
          if(millis() < 10000) 
          {
            drmStartPrint(version);
            if(serprt) Serial.print(micro_intv); if(serprt) Serial.print(" ");
            if(serprt) Serial.println(icnt);
            print_serial();
          }
          if(serprt) Serial.println();

          char stemp[20];
          sprintf(stemp, "%02d", myGPS.year);
          out = String(log_cnt++) + "\trtc\t20" + String(stemp);
          sprintf(stemp, "%02d", myGPS.month);
          out = out + String(stemp);
          sprintf(stemp, "%02d", myGPS.day);
          out = out + String(stemp) + "\t";

          sprintf(stemp, "%02d", myGPS.hour);
          out = out + String(stemp);
          sprintf(stemp, "%02d", myGPS.minute);
          out = out + String(stemp);
          sprintf(stemp, "%02d", myGPS.seconds);
          out = out + String(stemp) + ".";
          sprintf(stemp, "%03d", myGPS.milliseconds) + "\t";
          out = out + String(stemp) + "\t";

          out = out + String(val) + " V";
          out = out + "\r\n" + String(log_cnt++) + "\tgps\t";

          float fmin;
          long deg, imin, ifmin;
          char simin[4], sifmin[4];
          deg = (int) (myGPS.latitude/100);
          imin = (int) (myGPS.latitude - 100 * ((int) myGPS.latitude/100));
          sprintf(simin, "%02d", imin);
          fmin = myGPS.latitude - 100 * ((int) myGPS.latitude/100);
          ifmin = (int) 1000 * (fmin - (float) imin);
          sprintf(sifmin, "%03d", ifmin);
          out = out + String(deg) + ":" + simin + "." + sifmin + " " + myGPS.lat + "\t";

          deg = (int) (myGPS.longitude/100);
          imin = (int) (myGPS.longitude - 100 * ((int) myGPS.longitude/100));
          sprintf(simin, "%02d", imin);
          fmin = myGPS.longitude - 100 * ((int) myGPS.longitude/100);
          ifmin = (int) 1000 * (fmin - (float) imin);
          sprintf(sifmin, "%03d", ifmin);
          out = out + String(deg) + ":" + simin + "." + sifmin + " " + myGPS.lon;

          out = out + "\t" + 
                      String(myGPS.speed, 2) + " kt\t" +
                      String(myGPS.angle, 2) + " deg\t" +
                      String(myGPS.altitude, 2) + "m\tn-> " +
                      String(myGPS.satellites);
          if(serprt)
          {
            if(serprt) Serial.println(out);
          }
          if(wrt_ble && (bleprt || (icnt % BLEMOD == 0 && icnt > 10)))
          {
            bleprt=true;
            ble.println(out);
          }

          out = String(log_cnt++) + "\tmisc\t" + String(micro_intv) + "\t";
          out = out + String(((float)micro_intv) + micro_corr, 1) + "\t";
          out = out + String(micro_corr, 2) + "\t";
          out = out + String(icnt);
          if(serprt)
          {
            if(serprt) Serial.println(out);
          }
          if(bleprt)
          {
            ble.println(out);
          }
          
          digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
          bleprt=false;
          read9dof();
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
