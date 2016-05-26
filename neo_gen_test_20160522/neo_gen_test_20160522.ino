// 
// Neopixels with new ring and line parts from China drm 20160512
const char *version="neo_gen_test_20160522 -> V1.0-20160522 ";

// #include <Arduino.h>

// #include <avr/sleep.h>
// #include <avr/power.h>
// #include <avr/wdt.h>
// #include <Wire.h>
// #include <EEPROM.h>
#include <drmLib.h>

#include <Adafruit_NeoPixel.h>

// Defines for the neopixels
// #define PIN              8 // pin driving the neopixels ring
// #define NUMPIXELS       16 // How many NeoPixels in the ring
#define PIN              7 // pin driving the neopixels ring
#define NUMPIXELS        8 // How many NeoPixels in the ring
#define INTENMAX        40 // Maximum intensity
#define INTENINC        15 // Intensity Increment
#define DELAYBASE       1 // Base delay increment

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() 
{
  Serial.begin(115200); // I always use the serial for debug
  drmStartPrint(version);
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.clear(); pixels.show(); // clear all and then send it out
  pixels.show();
}

void loop() {
  // Loop through all addressable neopixels one at a time
  int p;
  for(p=0; p<NUMPIXELS; p++)
  {
    // show some sampleing of all colors
    int r, g, b;
    for(r=0; r<=INTENMAX; r+=INTENINC)
      for(g=0; g<=INTENMAX; g+=INTENINC)
        for(b=0; b<=INTENMAX; b+=INTENINC)
    {
          pixels.clear();
          pixels.setPixelColor(p, pixels.Color(r, g, b));
          pixels.show();
          if(DELAYBASE!=0) delay(DELAYBASE);
    }
  }
}
