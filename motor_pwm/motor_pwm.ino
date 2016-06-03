// Color Sensor
// drm 20160530
// V0.9 Just sketching out the rough outline
const char *version="motor_pwm -> V1.0-20160531 ";

#define FWD 5
#define REV 6

#include <drmLib.h>
volatile long count=0;
int OEstate=LOW;

void setup() 
{
  Serial.begin(115200); // I always use the serial for debug
  drmStartPrint(version);
  
  pinMode(FWD, OUTPUT);
  digitalWrite(FWD, LOW);

  pinMode(REV, OUTPUT);
  digitalWrite(REV, LOW);
}

void loop()
{
  long i=0;
  for(i; i<4; i++)
  {
    delay(500);
    analogWrite(FWD, i*50);
  }
}
