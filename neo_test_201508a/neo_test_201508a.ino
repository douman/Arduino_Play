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

const char *version="neo_test_201508a -> V0.0-20150814 ";
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      144

// Maximum intensity setting
#define INTENMAX 150

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
  pixels.clear();
}

void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  for(int n=1; n<8; n++)
  {
    int r, g, b;
    r=((n&1)!=0);
    g=((n&2)!=0);
    b=((n&4)!=0);    
    
    for(int m=1; m<min(255,INTENMAX); m+=29)
    {
      /*
      Serial.print("n=");
      Serial.print(n); Serial.print(", m=");
      Serial.print(m); Serial.print(" - r=");
      Serial.print(r); Serial.print(", g=");
      Serial.print(g); Serial.print(", b=");
      Serial.println(b);
      */
      
      for(int i=0; i<NUMPIXELS; i++){
    
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(min(r*m, INTENMAX), min(g*m, INTENMAX), min(b*m, INTENMAX))); // Moderately bright green color.
    
        pixels.show(); // This sends the updated pixel color to the hardware.
    
        delay(delayval); // Delay for a period of time (in milliseconds).
    
      } // i
    } // m
    pixels.clear();
    delay(delayval*100+100);
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
