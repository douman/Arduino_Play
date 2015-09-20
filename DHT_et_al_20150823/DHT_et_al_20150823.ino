// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#include <EEPROM.h>

#define DHTPIN 5     // DHT pin
#define ADC_DELAY 2 // delay between ADC measurements (ms)
#define DHT_DELAY 1500 // delay between DHT measurements (ms)

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

const char *version="DHT_el_al_20150823 -> V1.0-20150823 ";

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
// DHT dht(DHTPIN, DHTTYPE, 60); // added 3rd parameter "60" per note in adafuit support forums
                              // doesn't improve "good read rate

long dhtGood = 0, dhtBad = 0;

unsigned long start_millis;

void setup()
{
  start_millis = millis();
  Serial.begin(115200);
  // Serial.println("DHTxx test!");
  drm_Start_print();
  Serial.print("start_millis: "); Serial.println(start_millis);
  Serial.print("Starting at-> ");
  printTime(millis()); Serial.println();
  dht.begin();
  pinMode(A0,INPUT);
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    dhtBad++;
  }
  else
  {
    dhtGood++;
  
    // I really do not care about the heat index
    // Compute heat index in Fahrenheit (the default)
    /*
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);
    */
  
    Serial.println();
    printTime(millis());
    Serial.print("Good: ");
    Serial.print(dhtGood);
    Serial.print(" \t");
    Serial.print("Bad: ");
    Serial.println(dhtBad);

    Serial.print("Hum: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.println(" *F\t");
  }
  int sensorValue, numAvg=20;
  long sensorSum = 0;
  sensorSum = 0;

  // read and print the input on analog pin 0:
  for(int i=0; i<numAvg; i++)
  {
    sensorValue = analogRead(A0);
    Serial.print(sensorValue); Serial.print(" ");
    sensorSum = sensorSum + (long) sensorValue;
/*
    Serial.print(i);
    Serial.print(" - ");
    Serial.print("A{0}-> {"); 
    Serial.print(sensorValue); 
    Serial.print(" / ");
    Serial.print((33*(long)sensorValue)/10); 
    Serial.print("mV / ");
    Serial.print(((33*(long)sensorValue)/10)-500); 
    Serial.println("(deg C*10)}");
 */
    delay(ADC_DELAY);
  }
  Serial.println();
  
  float A0_mV = (3.45 * (float) sensorSum / (float) numAvg)/1.024; //adjusted ref voltage to calibrate temperature to Taylor digital thermometerf
  float tmp36_deg_c = (A0_mV/10.0) - 50.0;
  Serial.print("Avg");
  Serial.print(" - ");
  Serial.print("A{Avg}-> {"); 
  Serial.print((float)sensorSum/(float)numAvg); 
  Serial.print(" / ");
  Serial.print(A0_mV); 
  Serial.print("mV / ");
  Serial.print(" \t");
  Serial.print(tmp36_deg_c); 
  Serial.print(" deg C\t");
  Serial.print((tmp36_deg_c*9.0/5.0)+32.0);
  Serial.print(" deg F} Delta DHT vs TMP36 = ");
  Serial.println(t - tmp36_deg_c);

  // Wait a few seconds between measurements.
  delay(DHT_DELAY);
}
void printTime(unsigned long milli_time)
{
  const long ms_d =86400000; 
  const long ms_hr = 3600000;
  const long ms_min = 60000;
  const long ms_sec = 1000;
  unsigned long diff = milli_time - start_millis;
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
void drm_Start_print() 
{
  Serial.print(version); Serial.print(F(" SN#"));
  Serial.println(drm_Serialno());
  Serial.print(F("Compiled-> "));
  Serial.print(F(__DATE__)); 
  Serial.print(F(" "));
  Serial.println(F(__TIME__));
}
unsigned short drm_Serialno() {
  return(EEPROM.read(5) << 8 | EEPROM.read(6)); // combine two bytes into in serial number (drm specific)
}

