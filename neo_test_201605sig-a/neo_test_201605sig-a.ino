// 
// Neopixels with new ring and line parts from China drm 20160512
// V1.0 based on code from Adafruit
const char *version="neo_test_201605sig-a -> V1.1-20160522 ";

// #include <Arduino.h>

// #include <avr/sleep.h>
// #include <avr/power.h>
// #include <avr/wdt.h>
// #include <Wire.h>
// #include <EEPROM.h>
#include <drmLib.h>

#include <Adafruit_NeoPixel.h>

// Defines for the neopixels
#define PIN_R            8 // pin driving the neopixels ring
#define PIN_L            7 // pin driving the neopixels line
#define NUMPIXELS_R     16 // How many NeoPixels in the ring
#define NUMPIXELS_L      8 // How many NeoPixels in the line
#define INTENMAX 50        // Maximum intensity
#define DELAYBASE      200 // Base delay increment

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels_r = Adafruit_NeoPixel(NUMPIXELS_R, PIN_R, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_l = Adafruit_NeoPixel(NUMPIXELS_L, PIN_L, NEO_GRB + NEO_KHZ800);

void setup() 
{
  Serial.begin(115200); // I always use the serial for debug
  drmStartPrint(version);
  pixels_l.begin(); // This initializes the NeoPixel library.
  pixels_l.clear(); pixels_l.show(); // clear all and then send it out
  pixels_l.show();
  pixels_r.begin(); // This initializes the NeoPixel library.
  pixels_r.clear(); pixels_l.show(); // clear all and then send it out
  pixels_r.show();
}

void loop() {
  // Loop through state machine for traffic singal (R, G, Y)
  int color[]={INTENMAX, 0, 0};
  int p;
  int pstart=6;
  int pend=NUMPIXELS_L;
  int delayval=0;
while (1)
{
  for(int n=0; n<3; n++)
  {
    if(color[0]>1 && color[1]==0 && color[2]==0) // red -> green
    {
      color[0]=0; color[1]=INTENMAX;; color[2]=0;
      pstart=0; pend=min(1, NUMPIXELS_L);
      delayval=20*DELAYBASE;
    }
    else if(color[0]==0 && color[1]>0 && color[2]==0) // green -> yellow
    {
      color[0]=INTENMAX; color[1]=INTENMAX;; color[2]=0;
      pstart=3; pend=min(4, NUMPIXELS_L);
      delayval=3*DELAYBASE;
    }
    else if(color[0]>1 && color[1]>1 && color[2]==0) // yellow -> red
    {
      color[0]=INTENMAX; color[1]=0;; color[2]=0;
      pstart=6; pend=min(7, NUMPIXELS_L);
      delayval=8*DELAYBASE;
    }
    else 
    {
      color[0]=INTENMAX; color[1]=INTENMAX;; color[2]=INTENMAX; // white is error condition, should never occur
      delayval=DELAYBASE;
    }
    pixels_l.clear();
    for(p=pstart; p<=pend; p++) pixels_l.setPixelColor(p, pixels_l.Color(color[0], color[1], color[2]));
    pixels_l.show();

    if(delayval!=0) delay(delayval);
  }
}
}
