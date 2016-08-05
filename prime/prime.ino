/* Doug's silly prime number benchmark
  Written 6/19/88 for fun
  Written by Doug Manatt dou.man@gmail.com
  ported to Arduino 20160727 by drm
*/
#include <math.h>
  long divsor, hilm = 2000000000, lwlm = 100000001, count = 0;
  long num;
  boolean prime, restart = false;

void setup()
{
  Serial.begin(115200);
  // put your setup code here, to run once:
  num = lwlm;
  count = 0;
}

void loop() 
{
  if (num % 2 == 0) return;
  prime = true;
  for (divsor=3;divsor <= (long)sqrt ((double)num) && prime == true;divsor=divsor+2)
  {
    if (num%divsor == 0) prime = false;
  }
  if (prime)
  {
    Serial.print(num); Serial.print("  ");
    count++;
    if (count % 10 == 0) Serial.print("\n");
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
