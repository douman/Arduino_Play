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
#define PIN_L           A0 // pin driving the neopixels line
#define NUMPIXELS_R     16 // How many NeoPixels in the ring
#define NUMPIXELS_L      8 // How many NeoPixels in the line
#define INTENMAX        10 // Maximum intensity
#define DELAYBASE      40 // Base delay increment
#define GREENMULT        3 // green duration extension
#define REDMULT          3 // green duration extension
#define MAXSTATE         6 // maximum number of states

enum trafsig
{
  red,
  green,
  yellow
};
// 4 lit
// const int pstart[3] = {4, 0, 2};
// const int pend[3] = {7, 3, 5};

// 2 lit
const int pstart[3] = {6, 0, 3};
const int pend[3] = {7, 1, 4};

// 1 lit
// const int pstart[3] = {7, 0, 4};
// const int pend[3] = {7, 0, 4};

const int SIG_COLOR[3][3] = {{INTENMAX, 0, 0},         // red
                              {0, INTENMAX, 0},         // blue
                              {INTENMAX, INTENMAX, 0}}; // yellow

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels_r = Adafruit_NeoPixel(NUMPIXELS_R, PIN_R, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_l = Adafruit_NeoPixel(NUMPIXELS_L, PIN_L, NEO_GRB + NEO_KHZ800);

int light(enum trafsig sigcolor)
{
  int i;
  char color[7];
  switch(sigcolor)
  {
    case red:
      strncpy(color, "red", 7);
      i = 0;
      break;
    case green:
      strncpy(color, "green", 7);
      i = 1;
      break;
    case yellow:
      strncpy(color, "yellow", 7);
      i = 2;
      break;
    default:;
  }
  Serial.print("Signal ");
  Serial.println(color);
  pixels_l.clear();
  for(int p=pstart[i]; p<=pend[i]; p++) pixels_l.setPixelColor(p, pixels_l.Color(SIG_COLOR[i][0], SIG_COLOR[i][1], SIG_COLOR[i][2]));
  pixels_l.show();
}

int light_transtion(enum trafsig oldcolor, enum trafsig newcolor, int ttime)
{
  int newc, oldc;
  const int TINC=5;
  char ncolor[7], ocolor[7];
  switch(oldcolor)
  {
    case red:
      strncpy(ocolor, "red", 7);
      oldc = 0;
      break;
    case green:
      strncpy(ocolor, "green", 7);
      oldc = 1;
      break;
    case yellow:
      strncpy(ocolor, "yellow", 7);
      oldc = 2;
      break;
    default:;
  }
  switch(newcolor)
  {
    case red:
      strncpy(ncolor, "red", 7);
      newc = 0;
      break;
    case green:
      strncpy(ncolor, "green", 7);
      newc = 1;
      break;
    case yellow:
      strncpy(ncolor, "yellow", 7);
      newc = 2;
      break;
    default:;
  }
  Serial.print("Turning ");
  Serial.print(ocolor);
  Serial.print(" -to- ");
  Serial.println(ncolor);
  int cinc[3];
  for(int i=0; i<3; i++) cinc[i]=(SIG_COLOR[newc][i] - SIG_COLOR[oldc][i])/TINC;
  for(int t=0; t<ttime; t+=TINC)
  {
    pixels_l.clear();
    for(int p=pstart[newc]; p<=pend[newc]; p++) pixels_l.setPixelColor(p, pixels_l.Color(SIG_COLOR[newc][0], SIG_COLOR[newc][1], SIG_COLOR[newc][2]));
    for(int p=pstart[oldc]; p<=pend[oldc]; p++) pixels_l.setPixelColor(p, pixels_l.Color(SIG_COLOR[oldc][0] + cinc[0], SIG_COLOR[oldc][1] + cinc[1], SIG_COLOR[oldc][2] + cinc[2]));
    pixels_l.show();
    delay(ttime/TINC);    
  }
}

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

void loop() 
{
  int color[]={INTENMAX, 0, 0};
  int p;
  int delayval=0;
  while (1)
  {
    // Loop through state machine for traffic singal (R, G, Y)
    for(int state=0; state<MAXSTATE; state++)
    {
      switch (state) 
      {
      case 0:  // red light
        light(red); delayval=DELAYBASE*REDMULT;
        break;
      case 1:  // red to green
        light_transtion(red, green, 1);
        delayval=0;
        break;
      case 2:  // green light
        light(green); delayval=DELAYBASE*GREENMULT;
        break;
      case 3:  // green to yellow
        light_transtion(green, yellow, 1);
        delayval=0;
        break;
      case 4:  // yellow light
        light(yellow); delayval=DELAYBASE;
        break;
      case 5:  // yellow to red
        light_transtion(yellow, red, 1);
        delayval=0;
        break;
      default:;
      }
      if(delayval!=0) delay(delayval);
    }
  }
}
