#include <LiquidCrystal.h>

#include <avr/wdt.h>

void setup ()
{
  Serial.begin (115200);
  Serial.println ("Restarted.");
  wdt_enable (WDTO_1S);  // reset after one second, if no "pat the dog" received
 }  // end of setup

void loop ()
{
  Serial.println ("Entered loop ...");
  wdt_reset ();  // give me another second to do stuff (pat the dog)
  while (true) ;   // oops, went into a loop
}  // end of loop
