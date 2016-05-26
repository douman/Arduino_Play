// 
// Neopixels with new ring and line parts from China drm 20160512
// V1.0 based on code from Adafruit
const char *version="neo_test_201605a -> V1.1-20160522 ";

#include <Arduino.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <EEPROM.h>

#include <Adafruit_NeoPixel.h>

// Defines for the neopixels
#define PIN_R            8 // pin driving the neopixels ring
#define PIN_L            7 // pin driving the neopixels line
#define NUMPIXELS_R      16 // How many NeoPixels in the ring
#define NUMPIXELS_L      8 // How many NeoPixels in the line
#define INTENMAX 255 // Maximum intensity

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels_r = Adafruit_NeoPixel(NUMPIXELS_R, PIN_R, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_l = Adafruit_NeoPixel(NUMPIXELS_L, PIN_L, NEO_GRB + NEO_KHZ800);

int delayval = 50; // delay between writes

void setup() 
{
  Serial.begin(115200); // I always use the serial for debug
  drm_Start_print();
  pixels_l.begin(); // This initializes the NeoPixel library.
  pixels_l.clear(); pixels_l.show(); // clear all and then send it out
  pixels_r.begin(); // This initializes the NeoPixel library.
  pixels_r.clear(); pixels_l.show(); // clear all and then send it out
}

void loop() {

  // Loop through color changes
  for(int n=1; n<8; n++)
  {
    int r, g, b;
    r=((n&1)!=0);
    g=((n&2)!=0);
    b=((n&4)!=0);    
    
    // Loop through intensities
    for(int m=1; m<min(255,INTENMAX); m+=19)
    {
      Serial.print("m=");
      Serial.println(m);
      // unused debug printing
      //Serial.print("n=");
      //Serial.print(n); Serial.print(", m=");
      //Serial.print(m); Serial.print(" - r=");
      //Serial.print(r); Serial.print(", g=");
      //Serial.print(g); Serial.print(", b=");
      //Serial.println(b);
      
      int inc = 2; // the number of pixel in a packet
      int l1 = 0, dirl1 = 1; // keep track of the pixel
      int c1 = 0, dirc1 = 1; // keep track of the pixel
      int rval1, gval1, bval1;
      int l2 = NUMPIXELS_L-1, dirl2 = -1; // a second 
      int c2 = NUMPIXELS_R-1, dirc2 = -1; // a second 
      int rval2, gval2, bval2;
      int p;
      
      // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
      for(int i=0; i<max(NUMPIXELS_L,NUMPIXELS_R); i++) // move one pixel at a time
      {
        /*
        Serial.print("pixel=");
        Serial.print(pixel); Serial.print(", pixel2=");
        Serial.println(pixel2);
        */
        // Clear the previous pixels
        pixels_l.clear();
        pixels_r.clear();
       
        // packet 1 colors
        rval1 = min(r*m, INTENMAX); 
        gval1 = min(g*m, INTENMAX);
        bval1 = min(b*m, INTENMAX);
        // packet 2 colors
        rval2 = min(b*m, INTENMAX); 
        gval2 = min(r*m, INTENMAX);
        bval2 = min(g*m, INTENMAX);
        
        for(p=l1; abs(p-l1)<inc; p+=dirl1)
        {
// line
          if(p>=min(l2,l2+dirl2*inc) && p<=max(l2,l2+dirl2*inc))
          {
            pixels_l.setPixelColor(p, pixels_l.Color((rval1+rval2)/2, (gval1+gval2)/2, (bval1+bval2)/2)); // Turn pixel for packet 1,2 overlap
          }
          else
          {  
            pixels_l.setPixelColor(p, pixels_l.Color(rval1, gval1, bval1)); // Turn pixel for packet 1 only
          }
        }
// circle
        for(p=c1; abs(p-c1)<inc; p+=dirc1)
        {
          if(p>=min(c2,c2+dirc2*inc) && p<=max(c2,c2+dirc2*inc))
          {
            pixels_r.setPixelColor(p, pixels_r.Color((rval1+rval2)/2, (gval1+gval2)/2, (bval1+bval2)/2)); // Turn pixel for packet 1,2 overlap
          }
          else
          {
            pixels_r.setPixelColor(p, pixels_r.Color(rval1, gval1, bval1)); // Turn pixel for packet 1 only
          }
        } // p over pixels of packet 1
        
        for(p=l2; abs(p-l2)<inc; p+=dirl2)
        {
          if(!(p>=min(l1,l1+dirl1*inc) && p<=max(l1,l1+dirl1*inc))) // packet 2 only for not overlaping with packet 1
            pixels_l.setPixelColor(p, pixels_l.Color(rval2, gval2, bval2)); // Turn pixel for packet 2 only
        }

        for(p=c2; abs(p-c2)<inc; p+=dirc2)
        {
          if(!(p>=min(c1,c1+dirc1*inc) && p<=max(c1,c1+dirc1*inc))) // packet 2 only for not overlaping with packet 1
            pixels_r.setPixelColor(p, pixels_r.Color(rval2, gval2, bval2)); // Turn pixel for packet 2 only
        } // p over pixels of packet 2
        
        pixels_l.show(); // This sends the updated pixel color to the hardware.
        pixels_r.show(); // This sends the updated pixel color to the hardware.
        // increment things
        if(l1 == NUMPIXELS_L)
        {
          dirl1= -dirl1;
          dirl2= -dirl2;
          dirc1= -dirc1;
          dirc2= -dirc2;
        }
        l1+=dirl1;
        l2+=dirl2;
        c1+=dirc1;
        c2+=dirc2;
        if(delayval!=0) delay(delayval); // Delay for a period of time (in milliseconds).      
        } // i -- pixel motion
      } // m -- intensities
    pixels_l.clear(); pixels_l.show();
    pixels_r.clear(); pixels_r.show();
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
