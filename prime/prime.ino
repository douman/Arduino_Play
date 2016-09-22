/* Doug's silly prime number benchmark
  Written 6/19/88 for fun
  Written by Doug Manatt dou.man@gmail.com
  ported to Arduino 20160727 by drm
*/
#include <math.h>
#include <drmLib.h>
#define LED 13

const char *version="prime -> V5.0-20160805 ";

  long divsor, hilm = 2000000000, lwlm = 100000001, count = 0;
//  long divsor, hilm = 2000000000, lwlm = 1, count = 0;
  long num;
  boolean prime, restart = false;

void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  drmStartPrint(version);
  num = lwlm;
  count = 0;
}

void loop() 
{
  if (num % 2 == 0) num++;
  prime = true;
  for (divsor=3;divsor <= (long)sqrt ((double)num) && prime == true;divsor=divsor+2)
  {
    if (num%divsor == 0) prime = false;
  }
  if (prime)
  {
    digitalWrite(LED, true);
    Serial.print(num); Serial.print("\t");
    count++;
    if (count % 10 == 0) Serial.println();
    digitalWrite(LED, false);
  }
  num = num + 2;
  if(Serial.available())
  {
    char rchar = Serial.read();
    if(rchar == 'R' || rchar == 'r') restart = true;
  }
  if(num > hilm || restart)
  {
    Serial.print("\n");
    restart = false;
    setup();
  }
}
