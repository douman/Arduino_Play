// 
// generic testing drm 20160619
//
#include <drmLib.h>

#include <Adafruit_NeoPixel.h>

const char *version="test_20160619 -> V0.1-20160619 ";
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

// Maximum intensity setting
#define INTENMAX 150

// Setup the NeoPixel library
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 0; // delay between writes

void setup() 
{
  Serial.begin(115200);
  drmStartPrint(version);
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.clear();
}

void loop() 
{
  Serial.print("byteSize-> "); Serial.println(sizeof(byte));
  Serial.print("charSize-> "); Serial.println(sizeof(char));
  Serial.print("intSize-> "); Serial.println(sizeof(int));
  Serial.print("longSize-> "); Serial.println(sizeof(long));
  Serial.print("long longSize-> "); Serial.println(sizeof(long long));
  Serial.print("floatSize-> "); Serial.println(sizeof(float));
  Serial.print("doubleSize-> "); Serial.println(sizeof(double));
  Serial.print("loopsize-> "); Serial.println(sizeof(long));
  Serial.print("char* size-> "); Serial.println(sizeof(version));
  Serial.print("*versionsize-> "); Serial.println(sizeof(*version));
  Serial.print("NeoPixelsize-> "); Serial.println(sizeof(pixels));
  Serial.println();
  delay(5000);
}
