#include <drmLib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>
/*  Works with M0 Feather BLE drm 20160708
 *  This uses the Adafruit unified sensor library (Adafruit_Sensor),
*/
#define LED 13
const char *version="M0_Feather_LSM9DS0 -> V0.1-20160709 ";

/* Assign a unique base ID for this sensor */   
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(1000);  // Use I2C, ID #1000

//    Displays some basic information on this sensor from the unified
//    sensor API sensor_t type (see Adafruit_Sensor for more information)
void displaySensorDetails(void)
{
  sensor_t accel, mag, gyro, temp;
  
  lsm.getSensor(&accel, &mag, &gyro, &temp);
  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(accel.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(accel.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(accel.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(accel.max_value); Serial.println(F(" m/s^2"));
  Serial.print  (F("Min Value:    ")); Serial.print(accel.min_value); Serial.println(F(" m/s^2"));
  Serial.print  (F("Resolution:   ")); Serial.print(accel.resolution); Serial.println(F(" m/s^2"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));

  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(mag.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(mag.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(mag.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(mag.max_value); Serial.println(F(" uT"));
  Serial.print  (F("Min Value:    ")); Serial.print(mag.min_value); Serial.println(F(" uT"));
  Serial.print  (F("Resolution:   ")); Serial.print(mag.resolution); Serial.println(F(" uT"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));

  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(gyro.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(gyro.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(gyro.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(gyro.max_value); Serial.println(F(" rad/s"));
  Serial.print  (F("Min Value:    ")); Serial.print(gyro.min_value); Serial.println(F(" rad/s"));
  Serial.print  (F("Resolution:   ")); Serial.print(gyro.resolution); Serial.println(F(" rad/s"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));

  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(temp.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(temp.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(temp.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(temp.max_value); Serial.println(F(" C"));
  Serial.print  (F("Min Value:    ")); Serial.print(temp.min_value); Serial.println(F(" C"));
  Serial.print  (F("Resolution:   ")); Serial.print(temp.resolution); Serial.println(F(" C"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  
  delay(500);
}

//    Configures the gain and integration time for the TSL2561
void configureSensor(void)
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
}

unsigned long last_mclk = 0;
#define mclk_interval 2000
void setup(void) 
{  
  pinMode(LED, OUTPUT);
  delay(2000);
  Serial.begin(115200);
  drmStartPrint(version);
  
  /* Initialise the sensor */
  if(!lsm.begin())
  {
    /* There was a problem detecting the LSM9DS0 ... check your connections */
    Serial.print(F("Ooops, no LSM9DS0 detected ... Check your wiring or I2C ADDR!"));
    while(1);
  }
  Serial.println(F("Found LSM9DS0 9DOF"));
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Setup the sensor gain and integration time */
  configureSensor();
  
  /* We're ready to go! */
  Serial.println("");
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void) 
{  
  if((last_mclk + mclk_interval) > millis()) return;
  last_mclk = millis();
  int i;
  unsigned long startmilli = millis();
  for (i=0; i<3 ; i++)
  {
    /* Get a new sensor event */
    digitalWrite(LED, HIGH);
    sensors_event_t accel, mag, gyro, temp;
    lsm.getEvent(&accel, &mag, &gyro, &temp); 
    digitalWrite(LED, LOW);
    
    // print out accelleration data
    Serial.print("Accel X: "); Serial.print(accel.acceleration.x); Serial.print(" ");
    Serial.print("  \tY: "); Serial.print(accel.acceleration.y);       Serial.print(" ");
    Serial.print("  \tZ: "); Serial.print(accel.acceleration.z);     Serial.print("  \tm/s^2");
    Serial.print("  \tmag: "); Serial.println(sqrt(accel.acceleration.x*accel.acceleration.x + 
                                                   accel.acceleration.y*accel.acceleration.y + 
                                                   accel.acceleration.z*accel.acceleration.z));

    // print out magnetometer data
    Serial.print("Magn. X: "); Serial.print(mag.magnetic.x); Serial.print(" ");
    Serial.print("  \tY: "); Serial.print(mag.magnetic.y);       Serial.print(" ");
    Serial.print("  \tZ: "); Serial.print(mag.magnetic.z);     Serial.print("  \tgauss");
    Serial.print("  \tmag: "); Serial.println(sqrt(mag.magnetic.x*mag.magnetic.x + 
                                                   mag.magnetic.y*mag.magnetic.y + 
                                                   mag.magnetic.z*mag.magnetic.z));
    
    // print out gyroscopic data
    Serial.print("Gyro  X: "); Serial.print(gyro.gyro.x); Serial.print(" ");
    Serial.print("  \tY: "); Serial.print(gyro.gyro.y);       Serial.print(" ");
    Serial.print("  \tZ: "); Serial.print(gyro.gyro.z);     Serial.print("  \tdps");
    Serial.print("  \tmag: "); Serial.println(sqrt(gyro.gyro.x*gyro.gyro.x + 
                                                   gyro.gyro.y*gyro.gyro.y + 
                                                   gyro.gyro.z*gyro.gyro.z));
  
    // print out accelleration corrected data
    //Serial.print("Accel X: "); Serial.print(accel.acceleration.x); Serial.print(" ");
    //Serial.print("  \tY: "); Serial.print(accel.acceleration.y);       Serial.print(" ");
    //Serial.print("  \tZ: "); Serial.print(accel.acceleration.z);     Serial.println("  \tm/s^2");
  
    // print out magnetometer corrected data
    //Serial.print("Magn. X: "); Serial.print(mag.magnetic.x); Serial.print(" ");
    //Serial.print("  \tY: "); Serial.print(mag.magnetic.y);       Serial.print(" ");
    //Serial.print("  \tZ: "); Serial.print(mag.magnetic.z);     Serial.println("  \tgauss");
    
    // print out gyroscopic corrected data
    //Serial.print("Gyro-cX: "); Serial.print(gyro.gyro.x+1.9); Serial.print(" ");
    //Serial.print("  \tY: "); Serial.print(gyro.gyro.y+1);       Serial.print(" ");
    //Serial.print("  \tZ: "); Serial.print(gyro.gyro.z-0.5);     Serial.println("  \tdps");
    
    // print out temperature data
    Serial.print("Temp: "); Serial.print(temp.temperature); Serial.println(" *C");
  }

  Serial.print(millis() - startmilli);
  Serial.println(" ms **********************\n");
}
