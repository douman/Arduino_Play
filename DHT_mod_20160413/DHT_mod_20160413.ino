// Doug's DHT code for weather station -- based on code from:
// drm original DHT which was based upon:
// example written by ladyada, public domain -- modified heavily by drm 20151013

#include "DHT.h"
#include <drmLib.h>

#define LED_PIN 13
#define DHT_PIN 5     // DHT pin
#define DHT_DELAY 60000 // delay between DHT measurements (ms)

// Uncomment whatever type you're using!
//#define DHT_TYPE DHT11   // DHT 11
#define DHT_TYPE DHT22   // DHT 22  (AM2302)
//#define DHT_TYPE DHT21   // DHT 21 (AM2301)

const char *VER="DHT_mod_20160413 -> V0.9-20160413 ";

// V0.9 moving all DHT code into seperate procedures
// V0.9 blinks the LED when DHT read is successful
// V0.9 and now using drmLib for utility functions
// V0.9 in process, going to add RTC for time reading (maybe) and look into adding this to Weather Shield 

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHT_PIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// DHT22 works better with no 10k resistor (just the Arduino Pro Mini pullup) drm

// Initialize DHT sensor.
DHT dht(DHT_PIN, DHT_TYPE);
long dhtGood = 0, dhtBad = 0;

unsigned long start_millis, last_millis;
float tempf = 99.0, hum_pct = 99.0;

void setup()
{
  start_millis = millis();
  Serial.begin(115200);
  drmStartPrint(VER);
  
  DHT_setup();
  
  Serial.print("start_millis: "); Serial.println(start_millis);
  Serial.print("Starting at-> ");
  printTime(millis() - start_millis); Serial.println();
  dht.begin();
}

int DHT_setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // force the DHT pin low to start
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);

  delay(DHT_DELAY);

  pinMode(DHT_PIN, INPUT);
}

void loop() {
  
  DHT_read(); // update DHT values if available and the time is right

  Serial.println();
  printTime(millis() - start_millis);
  Serial.print("Good: ");
  Serial.print(dhtGood);
  Serial.print(" \t");
  Serial.print("Bad: ");
  Serial.println(dhtBad);

  Serial.print("Hum: ");
  Serial.print(hum_pct);
  Serial.print(" %\t");
  Serial.print("Temp: ");
  Serial.print(tempf);
  Serial.println(" *F\t");
}

int DHT_read()
{
  long curr_millis;
  curr_millis = millis();
  if(curr_millis - last_millis > DHT_DELAY || last_millis > curr_millis)
  // take reading if DHT_DELAY has elapsed or millis counter has overflowed
  {
    last_millis = curr_millis;
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    delay(DHT_DELAY);
    float f = dht.readTemperature(true);

    if(!isnan(h)) hum_pct = h;
    if(!isnan(f)) tempf = f;
    if (isnan(h) || isnan(f)) 
    {
      Serial.println("Failed to read from DHT sensor!");
      dhtBad++;
    }
    else dhtGood++;
  }
}
