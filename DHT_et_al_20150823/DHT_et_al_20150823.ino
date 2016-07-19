// Doug's DHT and TMP36 reader/reporter
// Derived from example written by ladyada, public domain -- modified heavily by drm 20151013

#include "DHT.h"
#include <drmLib.h>

#define LED_PIN 13
#define ADC_PIN A0
#define DHT_PIN 5     // DHT pin
#define ADC_DELAY 20 // delay between ADC measurements (ms)
#define DHT_DELAY 300 // delay between DHT measurements (ms)
// #define V_REF 3.45  // Reference voltage for 3.3V supply
// #define V_REF 3.425  // Reference voltage for 3.3V supply
#define V_REF 3.415  // Reference voltage for 3.3V supply
#define V_REF 1.04  // Reference voltage from Atmega328
// #define V_REF 5.0  // Reference voltage for 5V supply

// Uncomment whatever type you're using!
//#define DHT_TYPE DHT11   // DHT 11
#define DHT_TYPE DHT22   // DHT 22  (AM2302)
//#define DHT_TYPE DHT21   // DHT 21 (AM2301)

const char *VER="DHT_el_al_20150823 -> V1.8-20151220 ";

// V1.7 blinks the LED when DHT read is successful
// V1.8 adjusted to use internal reference (Value of 1.04 detirmined using Fluke DVM)
//      and now using drmLib for utility functions
// V2.0 in process, going to add RTC for time reading and look into adding this to Weather Shield
// it is assumed that the RTC is already going and acurate -- used read only

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHT_PIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// DHT22 works better with no 10k resistor (just the Arduino Pro Mini pullup) drm

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

// DHT dht(DHT_PIN, DHT_TYPE, 60); // added 3rd parameter "60" per note in adafuit support forums
                              // doesn't improve "good read rate drm 20151010
DHT dht(DHT_PIN, DHT_TYPE);

long dhtGood = 0, dhtBad = 0;

unsigned long start_millis;

void setup()
{
  start_millis = millis();
  Serial.begin(115200);
  drmStartPrint(VER);

  Wire.begin(); // initialize for RTC access

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);

  delay(5*DHT_DELAY);
/*  pinMode(DHT_PIN,INPUT_PULLUP);
  digitalWrite(DHT_PIN,HIGH);
 */

  pinMode(DHT_PIN, INPUT);
  pinMode(ADC_PIN, INPUT);
  analogReference(INTERNAL);
//  analogReference(EXTERNAL);
  Serial.print("start_millis: "); Serial.println(start_millis);
  Serial.print("Starting at-> ");
  printTime(millis() - start_millis); Serial.println();
  dht.begin();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  delay(DHT_DELAY);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  delay(DHT_DELAY);
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  delay(DHT_DELAY);
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
    digitalWrite(LED_PIN, HIGH); // good read, turn on the LED
  }
  int sensorValue, numAvg=20;
  long sensorSum = 0;
  sensorSum = 0;

  // read and print the input on analog pin 0:
  for(int i=0; i<numAvg; i++)
  {
    sensorValue = analogRead(ADC_PIN);
    sensorSum = sensorSum + (long) sensorValue;

    delay(ADC_DELAY/2);
  }
  
  Serial.println();
  printTime(millis() - start_millis);
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

  float ADC_PIN_mV = (V_REF * (float) sensorSum / (float) numAvg)/1.024; //adjusted ref voltage to calibrate temperature to Taylor digital thermometerf
  float tmp36_deg_c = (ADC_PIN_mV/10.0) - 50.0;
  Serial.print("Avg");
  Serial.print(" - ");
  Serial.print("A{Avg}-> {"); 
  Serial.print((float)sensorSum/(float)numAvg); 
  Serial.print(" / ");
  Serial.print(ADC_PIN_mV); 
  Serial.print("mV / ");
  Serial.print(" \t");
  Serial.print(tmp36_deg_c); 
  Serial.print(" deg C\t");
  Serial.print((tmp36_deg_c*9.0/5.0)+32.0);
  Serial.print(" deg F} Delta DHT vs TMP36 = ");
  Serial.println(t - tmp36_deg_c);
  digitalWrite(LED_PIN, LOW); // Turn off the LED (if the DHT read was good, it is on)
  // Wait a few seconds between measurements.
}
