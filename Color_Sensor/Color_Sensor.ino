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
volatile long count=0;
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
  pinMode(OUT, INPUT_PULLUP);
  digitalWrite(OUT, LOW);
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
  digitalWrite(OE, HIGH);
}

void loop()
{
  long all, cnt_val, i=0, red, grn, blu;
  for(i; i<4; i++)
  {
    count=0;
    delay(2000);
    digitalWrite(S3,!( i & B01));
    digitalWrite(S2, ! ((i & B10)>>1));
    Serial.print(i);
    Serial.print("-");
    Serial.print("-> ");
    cnt_val = count;
    switch (i)
    {
      case  0: Serial.print("Red   "); red=cnt_val; break;
      case  1: Serial.print("Blue  "); blu=cnt_val; break;
      case  2: Serial.print("Clear "); all=cnt_val; break;
      case  3: Serial.print("Green "); grn=cnt_val; break;
      default: Serial.print("What! "); break;
    }
    Serial.println(cnt_val);
  }
  Serial.println("  R   G   B   S");
  Serial.print(" ");
  Serial.print((100*red)/all); Serial.print("  ");
  Serial.print((100*grn)/all); Serial.print("  ");
  Serial.print((100*blu)/all); Serial.print("  ");
  Serial.print((100*(red+grn+blu))/all);
  Serial.println();
  // OEstate= ! OEstate;
  // if (OEstate==1) digitalWrite(OE, HIGH);
  // else digitalWrite(OE, LOW);
  // Serial.println(OEstate);
}
