#include "M0_GPS.h"

// These are for bluefruit
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

/* Utilitize the GPS with M0 Feather BLE
 *  with liberal code borrowing from Adafruit
 *  
 *  V0.9 by drm 20170809
 */

Adafruit_GPS myGPS(&Serial1);                  // Ultimate GPS FeatherWing
Adafruit_LSM9DS0 my9DOF = Adafruit_LSM9DS0();  // i2c 9DOF sensor
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); // Bluetooth LE
#include "M0_Output_Procs.h"

void pps_int()
{
  micro_end = micros();
  micro_intv = micro_end - micro_beg;
  micro_beg = micro_end;
  new_sec = true;
  ppscnt++;
}

void rtc_pps_int()
{
  rtc_sec_cnt++;
  rtc_32768_cnt=0;
}

void rtc_32768_int()
{
  rtc_latest_micro=micros();
  rtc_32768_cnt++;
}

// A small helper -- ADAFRUIT (not much help, drm)
void adaError(const __FlashStringHelper*err)
{
  if(serprt)
  {
    Serial.print("Fatal Error-looping-> ");
    Serial.println(err);
    while (1);
  }
}

int initNineDoF()
{
  /* Initialise the 9DOF sensor */
  if(!my9DOF.begin())
  {
    /* There was a problem detecting the LSM9DS0 ... check your connections */
    Serial.print(F("Ooops, no LSM9DS0 detected ... Check your wiring or I2C ADDR!"));
    while(1);
  }
  Serial.println(F("Found LSM9DS0 9DOF"));
  // 1.) Set the accelerometer range
  my9DOF.setupAccel(my9DOF.LSM9DS0_ACCELRANGE_2G);
  // 2.) Set the magnetometer sensitivity
  my9DOF.setupMag(my9DOF.LSM9DS0_MAGGAIN_2GAUSS);
  // 3.) Setup the gyroscope
  my9DOF.setupGyro(my9DOF.LSM9DS0_GYROSCALE_245DPS);
  return(ST_AOK);  
}

int initBLE()
{
  // Set up the Bluetooth LE
  if ( !ble.begin(VERBOSE_MODE) )
  {
    adaError(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("BLE OK!"));
    if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() )
    {
      adaError(F("Couldn't factory reset"));
    }
  }
  ble.verbose(false);  // debug info is a little annoying after this point!
  /* Disable command echo from Bluefruit */
  ble.echo(false);
  // Set module to DATA mode
  ble.setMode(BLUEFRUIT_MODE_DATA);
  return(ST_AOK);
}

int initGPS()
{
  // Set up the GPS
  pinMode(GPSPPSINT, INPUT);
  pinMode (GPSENABLE, OUTPUT);
  digitalWrite(GPSENABLE, LOW); // enable the GPS
  Serial1.begin(GPS_BAUD);
  myGPS.begin(GPS_BAUD);
//  myGPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ); // 5/sec fixes
//  myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ); // 5/sec updates
   myGPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ); // 1/sec fixes
   myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1/sec updates
  myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA); // all sentences
  // myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY); // only the R sentence
  
    // set up the GPS PPS interupt driver
  attachInterrupt(digitalPinToInterrupt(GPSPPSINT), pps_int, RISING);
  return(ST_AOK);
}

// the setup function runs once when you press reset or power the board
void setup() 
{
  Serial.begin(115200);
  delay(2000);
  drmStartPrint(version);
   
  // initialize digital pin (LED) 13 as an output and the battery level as analog input
  analogReadResolution(12);
  pinMode(LED, OUTPUT); digitalWrite(LED, LOW);
  pinMode(SAMM0_BATT, INPUT); // Battery level adc input

  /* Initialise the 9DOF sensor */
  initNineDoF();

  // Set up the Bluetooth LE
  initBLE();

  // Set up the GPS
  initGPS();
}

// Read the IMU sensor
void nineDoFProcess(void)
{
  lastNINEDoFmillis = millis();
  digitalWrite(LED, HIGH);
  sensors_event_t accel, mag, gyro, temp; // all local, no global knowledge of 9 DoF
  my9DOF.getEvent(&accel, &mag, &gyro, &temp); 
  digitalWrite(LED, LOW);

  // Write the data
//  String out = String(OUT_SIZE);
//
//  out = String(log_cnt++) + "\tacl\t" + String(accel.acceleration.x, 2) + "\t" + 
//                                        String(accel.acceleration.y, 2) + "\t" + 
//                                        String(accel.acceleration.z, 2) + "\t" +
//                                        String(sqrt(accel.acceleration.x * accel.acceleration.x + 
//                                                    accel.acceleration.y * accel.acceleration.y + 
//                                                    accel.acceleration.z * accel.acceleration.z), 2) + "\r\n" +
//        String(log_cnt++) + "\tgyr\t" + String(gyro.gyro.x, 2) + "\t" + 
//                                        String(gyro.gyro.y, 2) + "\t" + 
//                                        String(gyro.gyro.z, 2) + "\t" +
//                                        String(sqrt(gyro.gyro.x * gyro.gyro.x + 
//                                                    gyro.gyro.y * gyro.gyro.y + 
//                                                    gyro.gyro.z * gyro.gyro.z), 2) + "\r\n" +
//        String(log_cnt++) + "\tmag\t" + String(mag.magnetic.x, 2) + "\t" + 
//                                        String(mag.magnetic.y, 2) + "\t" + 
//                                        String(mag.magnetic.z, 2) + "\t" +
//                                        String(sqrt(mag.magnetic.x * mag.magnetic.x + 
//                                                    mag.magnetic.y * mag.magnetic.y + 
//                                                    mag.magnetic.z * mag.magnetic.z), 2);
//  if(serprt) Serial.println(out);
//}
//
//// print out the SAMD processor serial number
//void print_serial()
//{
//  long s[4], *ser;
//  ser = (long *) 0x0080A00C; // address of the processor serial number
//  int i;
//  for (i=3; i>=0; i--) s[i] = *ser++;
//
//  if(serprt) Serial.print("s ");
//  for(i=0; i<4; i++) if(serprt) Serial.print(s[i], HEX);
//  if(serprt) Serial.println();
}

void micro_clk_corr()
// Recalculate the 1sec microsecond correction (only for rational values of measured microseconds)
{
  if(new_sec && ppscnt > 10 && micro_intv > 999000 && micro_intv < 1001000)
  {
    long delta = 1000000 - micro_intv;
    if(ppscnt == 11) micro_corr = delta;
    micro_corr = (7.0 * micro_corr + ((float) delta))/8.0;
    new_sec = false; // is reset to true by interrupt on GPS PPS
  }
  return;  
}

int gpsProcess()
{
  String out = String(OUT_SIZE); // string for building the output string for GPS
  lastGPSmillis = millis();
  batt_volts = read_samM0_batt();
  char *sentence = myGPS.lastNMEA();
  float courserad = pi * myGPS.angle/180.0;
  float speedm_p_sec = myGPS.speed * m_p_sec_fac;
  x_sum = x_sum + speedm_p_sec * sin(courserad);
  y_sum = y_sum + speedm_p_sec * cos(courserad);
  if(serprt) {Serial.print("# "); Serial.println((sentence+1)); }
//  if(serprt)
//    {
//      // Serial.print(sentence[4]); // uniquely identify what kind of NMEA sentance
//      if (sentence[4] == 'R') {Serial.print("# R"); Serial.println(myGPS.satellites); }
//    }
//    // drm if (myGPS.fix && sentence[4] == 'R') // print only for "R" and we have a fix
//    {
//      // Format and print the parsed values
//      digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
//      char stemp[20];
//      sprintf(stemp, "%02d", myGPS.year);
//      out = String(log_cnt++) + "\trtc\t20" + String(stemp);
//      sprintf(stemp, "%02d", myGPS.month);
//      out = out + String(stemp);
//      sprintf(stemp, "%02d", myGPS.day);
//      out = out + String(stemp) + "\t";
//    
//      sprintf(stemp, "%02d", myGPS.hour);
//      out = out + String(stemp);
//      sprintf(stemp, "%02d", myGPS.minute);
//      out = out + String(stemp);
//      sprintf(stemp, "%02d", myGPS.seconds);
//      out = out + String(stemp) + ".";
//      sprintf(stemp, "%03d", myGPS.milliseconds) + "\t";
//      out = out + String(stemp) + "\t";
//    
//      out = out + String(batt_volts) + "\tV";
//      out = out + "\r\n" + String(log_cnt++) + "\tgps\t";
//    
//      float fmin;
//      long deg, imin, ifmin;
//      char simin[4], sifmin[4];
//      deg = (int) (myGPS.latitude/100);
//      imin = (int) (myGPS.latitude - 100 * ((int) myGPS.latitude/100));
//      sprintf(simin, "%02d", imin);
//      fmin = myGPS.latitude - 100 * ((int) myGPS.latitude/100);
//      ifmin = (int) 1000 * (fmin - (float) imin);
//      sprintf(sifmin, "%03d", ifmin);
//      out = out + String(deg) + "\t" + simin + "." + sifmin + "\t";
//    
//      deg = (int) (myGPS.longitude/100);
//      imin = (int) (myGPS.longitude - 100 * ((int) myGPS.longitude/100));
//      sprintf(simin, "%02d", imin);
//      fmin = myGPS.longitude - 100 * ((int) myGPS.longitude/100);
//      ifmin = (int) 1000 * (fmin - (float) imin);
//      sprintf(sifmin, "%03d", ifmin);
//      out = out + String(deg) + "\t" + simin + "." + sifmin + "\t" + myGPS.lat + myGPS.lon;
//    
//      out = out + "\t" + 
//                  String(myGPS.speed, 2) + "\tkt\t" +
//                  String(myGPS.angle, 2) + "\tdeg\t" +
//                  String(myGPS.altitude, 2) + "\t" +
//                  String(myGPS.satellites);
//      if(serprt)
//      {
//        if(serprt) Serial.println(out);
//      }
//      if(wrt_ble && (ppscnt % BLEMOD == 0 && ppscnt > 10))
//      {
//        bleprt=true;
//        ble.println(out);
//      }
//    
//      out = String(log_cnt++) + "\tmisc\t" + String(micro_intv) + "\t" +
//            String(((float)micro_intv) + micro_corr, 1) + "\t" +
//            String(micro_corr, 2) + "\t" +
//            String(ppscnt) + "\t" + 
//            String(millis()) + "\t" + 
//            String(micros());
//      if(serprt)
//      {
//        if(serprt) Serial.println(out);
//      }
//      if(bleprt)
//      {
//        ble.println(out);
//      }
//    }
//    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
//    bleprt=false;
  return(ST_AOK);
}

// the loop function runs over and over again forever
void loop() 
{
  interrupts(); // Make sure interrupts are on

  // accept simple commands on serial input
  if (Serial.available()) // 'b' or 'B' toggles bluetooth output 's' or 'S' toggles serial output
  {
    char c = Serial.read();
    if(c == 'B' || c == 'b') 
    { 
      bleprt = wrt_ble = !wrt_ble; 
      Serial.print("# BLE swap "); 
      if(wrt_ble) Serial.print(" TRUE "); 
      Serial.println(wrt_ble); 
    }
    if(c == 'S' || c == 's') 
    { 
      serprt = !serprt; 
      Serial.print("# SER swap "); 
      Serial.println(serprt); 
    }
  }
/*  
 * Figure out logic to turn on BLE output from monitoring BLE input (previous code did not work)
 */
  
  // Recalculate the 1sec microsecond correction (only for rational values of measured microseconds)
  micro_clk_corr();

  unsigned long temp_millis = millis();
  // Things to do only in early cycles
  if((temp_millis < 10000) && (temp_millis - 1000 > serprt_millis)) // only early on
      {
        serprt_millis = temp_millis;
        Serial.print("# SerBuf0-> ");
        Serial.println(Serial.availableForWrite());
        drmStartPrint(version);
        Serial.print("# SerBuf1-> ");
        Serial.println(Serial.availableForWrite());
        if(serprt) { Serial.print("# "); Serial.print(micro_intv); Serial.print(" "); Serial.println(ppscnt); }
        // Print Serial number
        print_samM0_serial();
      }

  // Process GPS
  
  boolean gpsGo = myGPS.newNMEAreceived();
  char *sentence = myGPS.lastNMEA();/* && (lastGPSmillis + GPSmillis < millis()) && (sentence[4] == 'R') */ ; // Messing with this compare drm 20161204
  gpsGo = gpsGo /* && (sentence[4] == 'R') */;
  if (gpsGo) 
  {
    
    gpsProcess();
//    printXY();
//    delay(DELAY);
  }
  
  // Process 9 DoF
  boolean nineDoFGo = lastNINEDoFmillis + NINEDoFmillis < millis(); 
  if (nineDoFGo) nineDoFProcess();
  
  // process characters from GPS
  if (Serial1.available())
  {
    char c = myGPS.read();
    
    if (c=='$')
    {
#ifdef DEBUG
      if(serprt) Serial.println(myGPS.newNMEAreceived());
      if(serprt) Serial.print("Chsum-> ");
      if(serprt) Serial.println(savecksum, HEX);
      if(serprt) Serial.print(" - Fx: "); if(serprt) Serial.print((int)myGPS.fix);
      if(serprt) Serial.print(" quality: "); if(serprt) Serial.println((int)myGPS.fixquality);
#endif
      }


    if (c=='$') // end of a GPS sentence
    {
      // Start for Calculating sentence checksum
      savecksum = cksum;
      cksum = 0;
      myGPS.parse(myGPS.lastNMEA());
    }
    else if (c!='*')
    {
      cksum = cksum ^ c; // xor for checksum calculation
    }
  }
}
