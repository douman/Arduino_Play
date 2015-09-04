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
#define NUMPIXELS      144

// Maximum intensity setting
#define INTENMAX 128

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 0; // delay between writes

void setup() 
{
  Serial.begin(115200);
  drm_Start_print();
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.clear(); pixels.show(); // clear all and then send it out
}

void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  // Loop through color changes
  for(int n=1; n<8; n++)
  {
    int r, g, b;
    r=((n&1)!=0);
    g=((n&2)!=0);
    b=((n&4)!=0);    
    
    // Loop through intensities
    for(int m=23; m<min(255,INTENMAX); m+=222)
    {
      Serial.print("m=");
      Serial.println(m);
      // unused debug printing
      Serial.print("n=");
      Serial.print(n); Serial.print(", m=");
      Serial.print(m); Serial.print(" - r=");
      Serial.print(r); Serial.print(", g=");
      Serial.print(g); Serial.print(", b=");
      Serial.println(b);
      
      int inc = 10; // the number of pixel in a packet
      int p1 = 0, dir1 = 1; // keep track of the pixel
      int rval1, gval1, bval1;
      int p2 = NUMPIXELS-1, dir2 = -1; // a second 
      int rval2, gval2, bval2;
      int p;
      
      for(int i=0; i<NUMPIXELS; i++) // move one pixel at a time
      {
        /*
        Serial.print("pixel=");
        Serial.print(pixel); Serial.print(", pixel2=");
        Serial.println(pixel2);
        */
        // Clear the previous pixels
        pixels.clear();
        
        // packet 1 colors
        rval1 = min(r*m, INTENMAX); 
        gval1 = min(g*m, INTENMAX);
        bval1 = min(b*m, INTENMAX);
        // packet 2 colors
        rval2 = min(b*m, INTENMAX); 
        gval2 = min(r*m, INTENMAX);
        bval2 = min(g*m, INTENMAX);
        
        for(p=p1; abs(p-p1)<inc; p+=dir1)
        {
          if(p>=min(p2,p2+dir2*inc) && p<=max(p2,p2+dir2*inc))
            pixels.setPixelColor(p, pixels.Color((rval1+rval2)/2, (bval1+bval2)/2, (bval1+bval2)/2)); // Turn pixel for packet 1,2 overlap
          else
            pixels.setPixelColor(p, pixels.Color(rval1, bval1, bval1)); // Turn pixel for packet 1 only
        } // p over pixels of packet 1
        
        for(p=p2; abs(p-p2)<inc; p+=dir2)
        {
          if(!(p>=min(p1,p1+dir1*inc) && p<=max(p1,p1+dir1*inc))) // packet 2 only for not overlaping with packet 1
            pixels.setPixelColor(p, pixels.Color(rval2, bval2, bval2)); // Turn pixel for packet 2 only
        } // p over pixels of packet 2
        
        pixels.show(); // This sends the updated pixel color to the hardware.
        // increment things
        if(p1 == NUMPIXELS)
        {
          dir1= -dir1;
          dir2= -dir2;
        }
        p1+=dir1;
        p2+=dir2;
        if(delayval!=0) delay(delayval); // Delay for a period of time (in milliseconds).      
        } // i -- pixel motion
      } // m -- intensities
    pixels.clear(); pixels.show();
    delay(25);
  } // n -- colors
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
