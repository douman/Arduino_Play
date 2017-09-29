const char *VERSION="ESP8266_Blink -> V1.0-20170925 ";
const long BAUD = 115200;
#include <drmLib.h>

void setup() {
  pinMode(0, OUTPUT);
  Serial.begin(BAUD);
  //drmStartPrint(VERSION);
  
}

void loop() {
  digitalWrite(0, HIGH);
  delay(100);
  digitalWrite(0, LOW);
  delay(100);
  Serial.println("Next!");
}
