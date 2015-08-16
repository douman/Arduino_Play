/*
  Table_Read_Writer, With a watchdog 
  modified by drm 20141011 to learn Arduino
 */
#include <avr/wdt.h>
#include <Wire.h>
#include <EEPROM.h>

const byte led = 13;
const byte dchst = 5;
const byte dchs = 8;
const byte achs = 8;
const char *version="Table_Read_Writer_wDig -> V1.3.1-20141024";

// the setup routine runs once when you press reset:
void setup() {
//  Wire.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);  
  watchdogSetup();
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  // Serial.begin(1200);
  Serial.print(version); Serial.print(" - SN#");
  Serial.println(drm_serialno());
  for(int m=dchst; m<dchs+dchst; m++) pinMode(m, INPUT_PULLUP);
}

char str[12];
unsigned long i=0;
byte ch_idx=0;
unsigned short ms_frac;
short stat; 
long frac;
unsigned long usec_time;
unsigned long last_usec_time;
long usec_delta;
long msec;
long sec;

// Define the channel descriptor and data structures
typedef struct {
  char func;
  byte pin;
  unsigned short ms_period;
  unsigned long last_msec;
  unsigned short avg_cnt;
  unsigned short cnt;
  long asum;
  short lastval;
  boolean on_chg;
  byte next_ch;
} IoT_ch;

IoT_ch ch[5] = {
  {'A', A0, 900, 0, 0, 0, 0, -1, true, 1} ,
  {'A', A1, 1700, 0, 0, 0, 0, -1, true, 2},
  {'A', A2, 2000, 0, 0, 0, 0, -1, true, 3},
  {'A', A3, 2130, 0, 0, 0, 0, -1, true, 4},
  {'I',  0, 10000, 0,  0, 0, 0, -1, false, 0}
//  {'I',  0, 1, 0,  0, 0, 0, -1, true, 0}
};
  
// the loop routine runs over and over again forever:
void loop() {
  wdt_reset();
  usec_time = micros();
  msec = usec_time/1000;
  sec = usec_time/1000000;

  if(last_usec_time > usec_time) ch[ch_idx].last_msec = 0; // when the timer wraps, zero things
  usec_delta = usec_time - 1000*(ch[ch_idx].last_msec + ch[ch_idx].ms_period);
  
  if (usec_delta >= 0) {
      /*      Serial.print(msec); Serial.print(" / ");
            Serial.print(ch[ch_idx].last_msec); Serial.print(" / ");
            Serial.print(ch[ch_idx].ms_period); Serial.print(" / ");
            Serial.println("");
       */
    frac = usec_time - sec * 1000000;
    ms_frac = 1000 + frac/1000; // convert to msec + 1000
    stat = process_chan(ch_idx);
    ch[ch_idx].last_msec += ch[ch_idx].ms_period;
  }
    ch_idx = ch[ch_idx].next_ch;
//    Serial.println("");Serial.print("*"); Serial.print(ch_idx);Serial.print(ch[ch_idx].func); Serial.println("*");
//    delay(200);
}

short process_chan(byte j) {
  unsigned short d_in;
  digitalWrite(led, HIGH);
//  Serial.print("*"); Serial.print(j);Serial.print(ch[j].func); Serial.print("*");
  switch (ch[j].func) {
    case 'A': a_chan(j); break;
    case 'I': d_in = di_chan(j); break;
    case 'O': do_chan(j); break;
    default:;
  }
  digitalWrite(led, LOW);
  last_usec_time = usec_time;
//  delay(100);
}

unsigned short di_chan(byte j) {
  boolean inval[dchs];
  unsigned short retval=0;
  for(int m=0; m<dchs; m++)  inval[m] = digitalRead(m+dchst);
  for(int m=dchs-1; m>=0; m--) {retval=retval << 1; if(inval[m]) retval |= 1;}
  if (ch[j].on_chg == false || retval != ch[j].lastval ) {
    hdr_prt(j);
    Serial.print("Digital-> Dpin:"); Serial.print(dchst); Serial.print("-"); Serial.print(dchst+dchs); Serial.print(" ");
    Serial.print(retval);
    Serial.println("");

//      Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
//      Wire.write((byte)0x0f); // start at register 0x0F Status Register
//      Wire.endTransmission();
//      Wire.requestFrom(0x68, 1); // Read one byte only
//      byte RTC_status = Wire.read();

//      Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
//      Wire.write((byte)0x0f); // Status Register
//      Wire.write((RTC_status & (byte) 0xFC)); // clear the alarm flags
//      Wire.endTransmission();      


    ch[j].lastval = retval;
  }
  return (retval);
}
void do_chan(byte j) {
}

void a_chan(byte j) {
  short ain;
  ain = analogRead(ch[j].pin);
  ch[j].asum = ch[j].asum + ain;
  ch[j].cnt++;
  if (ch[j].cnt >= ch[j].avg_cnt) {
    if (! ch[j].on_chg || ch[j].lastval != ain) {
      if (ch[j].avg_cnt > 0) ain = ch[j].asum/ch[j].avg_cnt;
      hdr_prt(j);
      Serial.print("Analog-> Apin:");
      Serial.print(ch[j].pin); Serial.print("-> ");
      Serial.print(ain);
      if (ch[j].avg_cnt >1 ) {
        Serial.print(" Avg-> ");
        Serial.print(ain); Serial.print(".");
        ch[j].asum = (ch[j].asum % ch[j].avg_cnt)*1000;
        sprintf(str,"%4d",1000+(ch[j].asum/ch[j].avg_cnt));
        Serial.print(str+1); /* strip off the leading 1 to get frac */
      }
      Serial.println("");
      ch[j].lastval = ain;
    }
    ch[j].cnt = 0;
    ch[j].asum = 0;
  }
}

void hdr_prt(byte j) {
  Serial.print(i); Serial.print(": "); i++;
  Serial.print(sec);
  Serial.print(".");
  sprintf(str,"%4d",ms_frac);
  Serial.print(str+1); /* strip off the leading 1 to get msecs */
  Serial.print(" Ch-> "); Serial.print(j); Serial.print(" / ");
  Serial.print("delta-> "); 
  Serial.print(msec - (ch[j].last_msec + ch[j].ms_period));
  Serial.print(" / "); Serial.print(usec_delta); Serial.print(" ");
}

void watchdogSetup(void)
{
  cli();
  wdt_reset();

/*WDTCSR configuration:
  WDIE = 1: Interrupt Enable
  WDE = 1 :Reset Enable
  See table for time-out variations:
  WDP3 = 0, WDP2 = 1, WDP1 = 1, WDP0 = 1 :For 2000ms Time-out
  WDP3 = 0, WDP2 = 1, WDP1 = 1, WDP0 = 0 :For 1000ms Time-out
  WDP3 = 1, WDP2 = 0, WDP1 = 0, WDP0 = 0 :For 1000ms Time-out */
  
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
  sei();
}

short drm_serialno() {
  return(EEPROM.read(5) << 8 | EEPROM.read(6));
}
