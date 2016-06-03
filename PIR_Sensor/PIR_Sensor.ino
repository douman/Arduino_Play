// PIR Sensor Test
// drm 20150602
// V0.9
#include <drmLib.h>

const char *version="PIR_Sensor -> V0.9-201600602 ";

#define IN_PIN      8
#define LED_PIN     13

int prev_state=0;
long unsigned prev_time;
void setup() 
{
  Serial.begin(115200); // I always use the serial for debug
  drmStartPrint(version);
  pinMode(IN_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() 
{
  int state = digitalRead(IN_PIN);
  digitalWrite(LED_PIN, state);
  if (state != prev_state)
  {
    unsigned long mtime = millis();
    Serial.print(mtime/1000);
    Serial.print(".");
    drmPrtLead0((long) mtime % 1000, 3);
    if (state == 1)
    {
      Serial.print(": Motion detected -> ");
      prev_time = mtime;
    }
    else
    {
      Serial.print(": Still again time-> ");
      Serial.print(mtime - prev_time);
      Serial.print(" ms -> ");
    }
    Serial.println(state);
    prev_state = state;
  }
  delay(1);
}
