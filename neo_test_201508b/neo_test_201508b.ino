// 
// Initial tests of neopixels by drm 20150814
// V0.0 based on code from Adafruit
//

// NeoPixel Ring simple sketch (c) 2013 Shae Erisson 2015 mods by drm
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Arduino.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <Wire.h>
#include <EEPROM.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

const char *version="neo_test_201508b -> V1.0-20150815 ";
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      143

// Maximum intensity setting
#define INTENMAX 255

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 500; // delay between writes

void setup() 
{
  Serial.begin(115200);
  drm_Start_print();
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.clear(); pixels.show();
}

void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  for(int n=1; n<8; n++)
  {
    int r, g, b;
    r=((n&1)!=0);
    g=((n&2)!=0);
    b=((n&4)!=0);    
    
    for(int m=23; m<min(255,INTENMAX); m+=222)
    {
      Serial.print("m=");
      Serial.println(m);
      /*
      Serial.print("n=");
      Serial.print(n); Serial.print(", m=");
      Serial.print(m); Serial.print(" - r=");
      Serial.print(r); Serial.print(", g=");
      Serial.print(g); Serial.print(", b=");
      Serial.println(b);
      */
      
      int pixel = 0, inc = 1, lastpixel=0;
      int pixel2=NUMPIXELS-1, inc2 = -1, lastpixel2=NUMPIXELS-1;
      int rval, gval, bval;
      for(int k=0; k<2; k++)
      {
        for(int i=0; i<NUMPIXELS; i++){
          /*
          Serial.print("pixel=");
          Serial.print(pixel); Serial.print(", pixel2=");
          Serial.println(pixel2);
          */
          
          rval = min(r*m, INTENMAX); 
          gval = min(g*m, INTENMAX);
          bval = min(b*m, INTENMAX);
          // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
          if(pixel != pixel2) pixels.setPixelColor(pixel, pixels.Color(rval, gval, bval)); // Turn pixel on
          if(lastpixel != pixel2) pixels.setPixelColor(lastpixel, 0, 0, 0); // Turn pixel off
          lastpixel = pixel;
          pixel+=inc;
          if(i==NUMPIXELS-1) inc=-inc;
          
          rval = min(b*m, INTENMAX); 
          gval = min(r*m, INTENMAX);
          bval = min(g*m, INTENMAX);
          
          if(lastpixel == pixel2)
          {
            rval = (min(r*m, INTENMAX) + rval)/2; 
            gval = (min(g*m, INTENMAX) + gval)/2;
            bval = (min(b*m, INTENMAX) + bval)/2;
          }
          
          pixels.setPixelColor(pixel2, pixels.Color(rval, gval, bval)); // Turn pixel on (colors swapped)
          if (lastpixel2 != lastpixel) pixels.setPixelColor(lastpixel2, 0, 0, 0); // Turn pixel off
          lastpixel2 = pixel2;
          pixel2+=inc2;
          if(i==NUMPIXELS-1) inc2=-inc2;
          
          pixels.show(); // This sends the updated pixel color to the hardware.
      
          // delay(delayval); // Delay for a period of time (in milliseconds).
      
        } // i
      } // k
    } // m
    pixels.clear(); pixels.show();
    delay(25);
  }
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
