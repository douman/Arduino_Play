// Color Sensor
// drm 20160530
// V0.9 Just sketching out the rough outline
const char *version="Color_Sensor -> V1.0-20160530 ";
#define OUT 3
#define S0 A3
#define S1 A4
#define S2 A1
#define S3 A2
#define OE A5

#include <drmLib.h>
volatile long count;
int OEstate=LOW;

void pin3cnt()
{
  count++;
}

void setup() 
{
  Serial.begin(115200); // I always use the serial for debug
  drmStartPrint(version);
  
  count=0;
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), pin3cnt, FALLING);
  
  pinMode(S0, OUTPUT);
  digitalWrite(S0, HIGH);
  pinMode(S1, OUTPUT);
  digitalWrite(S1, HIGH);

  pinMode(S2, OUTPUT);
  digitalWrite(S2, HIGH);
  pinMode(S3, OUTPUT);
  digitalWrite(S3, HIGH);
  pinMode(OE, OUTPUT);
  digitalWrite(OE, LOW);
}

void loop()
{
  int i=0;
  for(i; i<4; i++)
  {
    digitalWrite(S2,!( i & B01));
    digitalWrite(S3, ! ((i & B10)>>1));
    Serial.print(i);
    Serial.print("-");
    switch (i)
    {
      case  0: Serial.print("Red  "); break;
      case  1: Serial.print("Blue "); break;
      case  2: Serial.print("Clear"); break;
      case  3: Serial.print("Green"); break;
      default: Serial.print("What!"); break;
    }
    Serial.print("-");
    delay(100);
    Serial.println(count);
    count=0;
  }
  Serial.println();
  OEstate= ! OEstate;
  if (OEstate==1) digitalWrite(OE, HIGH);
  else digitalWrite(OE, LOW);
  Serial.println(OEstate);
}
