/* Test the YwRobot (chinese clone) 4x relay board
 
 derived by drm 20150823
 */
const char *VERSION="Relay_4x_Test -> V0.9b-20160418 ";
const long BAUD = 115200;

#include <drmLib.h>

#define RELAYBASE 8    //Pin for Relay First

unsigned long start_millis, rand_del[4] = {0,0,0,0};
long ledoncnt=0;
void setup() 
{
  Serial.begin(BAUD);
  drmStartPrint(VERSION);

  // set the digital pin as output
  int i;
  for (i=0; i<4; i++) { pinMode(RELAYBASE + i, OUTPUT); digitalWrite(RELAYBASE + i, HIGH); }
  start_millis = millis();
}

unsigned long prev_millis[4] = {0, 0, 0, 0};
void loop()
{
  // check to see if it's time to switch a relay (randomly)
  unsigned long curr_millis = millis();
  int i;
  for (i=0; i<4; i++)
  {
    if(curr_millis - prev_millis[i] >= rand_del[i]) 
    {
      int relay = RELAYBASE + i;
      int state = digitalRead(relay);
      if(state == LOW)
      {
        diag_relay(i, relay, state, ledoncnt);
        digitalWrite(relay, HIGH);
        ledoncnt--;
      }
      else // state == HIGH
      {
        if(ledoncnt < 4)
        {
          diag_relay(i, relay, state, ledoncnt);
          digitalWrite(relay, LOW);
          ledoncnt++;
        } 
      }
      rand_del[i] = random(50, 20000);
      prev_millis[i] = millis();
      Serial.print("Chan # "); Serial.print(i); Serial.print(" New Delay-> "); Serial.println(rand_del[i]);
    }
  }
  
}
int diag_relay(int i, int relay, int state, long ledoncnt)
{
  Serial.print("Diag i-> "); 
  Serial.print(i); 
  Serial.print(" relay-> "); 
  Serial.print(relay); 
  Serial.print(" state-> "); 
  Serial.print(state); 
  Serial.print(" ledoncnt-> "); 
  Serial.println(ledoncnt);
}

