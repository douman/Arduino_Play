#include <drmLib.h>

const char *ver="Lib_Test -> V1.0.0-20151214 ";

void setup() 
{
  Serial.begin(115200);
  drmStartPrint(ver);
}

void loop() {
  // put your main code here, to run repeatedly:

}
