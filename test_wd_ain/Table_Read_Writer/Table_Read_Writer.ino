/*
  Table_Read_Writer, With a watchdog 
  modified by drm 20141011 to learn Arduino
 */
 
#include <avr/wdt.h>
const int led = 13;
const char *version="Table_Read_Writer -> V1.0.0-20141018";

// the setup routine runs once when you press reset:
void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);  
  watchdogSetup();
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println(version);
}

char str[12];
unsigned long i=0;
byte ch_idx=0;
unsigned short ms_frac;
short stat; 
long frac;
unsigned long usec_time;
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
  byte on_chg;
  byte next_ch;
} IoT_ch;

IoT_ch ch[4] = {
  {'A', A0, 103, 0,  100, 0, 0, 0, true, 1},
  {'A', A1, 333, 0,  4, 0, 0, 0, true, 2},
  {'A', A2,  1000, 0, 3, 0, 0, 0, true, 3},
  {'A', A3, 210, 0, 500, 0, 0, 0, true, 0}
};
  
// the loop routine runs over and over again forever:
void loop() {
  wdt_reset();
  usec_time = micros();
  msec = usec_time/1000;
  sec = usec_time/1000000;

//  if (msec >= (ch[ch_idx].last_msec + ch[ch_idx].ms_period)) {
  usec_delta = usec_time - 1000*(ch[ch_idx].last_msec + ch[ch_idx].ms_period);
  if (usec_delta > 2000000000) ch[ch_idx].last_msec = 0; // reset when usec counter rolls over
  if (usec_delta >= 0) {
/*    Serial.print(msec); Serial.print(" / ");
    Serial.print(ch[ch_idx].last_msec); Serial.print(" / ");
    Serial.print(ch[ch_idx].ms_period); Serial.print(" / ");
    Serial.println("");
 */
    frac = usec_time - sec * 1000000;
    ms_frac = 1000 + frac/1000; // convert to msec + 1000
    stat = process_chan(ch_idx);
    ch[ch_idx].last_msec =ch[ch_idx].last_msec + ch[ch_idx].ms_period;
  }
    ch_idx = ch[ch_idx].next_ch;
//    delay(100);
}

short process_chan(byte j) {
  
  digitalWrite(led, HIGH);  

  if (ch[j].func = 'A')
    a_chan(j);
  else if (ch[j].func = 'I')
    di_chan(j);
  else if (ch[j].func = 'O')
    do_chan(j);
    
  digitalWrite(led, LOW);
}

void di_chan(byte j) {
}
void do_chan(byte j) {
}

void a_chan(byte j) {
  short ain;
  ain = analogRead(ch[j].pin);
  ch[j].asum = ch[j].asum + ain;
  ch[j].cnt++;
  if (ch[j].cnt >= ch[j].avg_cnt) {
    Serial.print(i); Serial.print(": "); i++;
    Serial.print(sec);
    Serial.print(".");
    sprintf(str,"%4d",ms_frac);
    Serial.print(str+1); /* strip off the leading 1 to get msecs */
    Serial.print(" Ch-> "); Serial.print(j); Serial.print(" / ");
    Serial.print("delta-> "); 
    Serial.print(msec - (ch[j].last_msec + ch[j].ms_period));
    Serial.print(" / "); Serial.print(usec_delta); Serial.print("   ");
    

    Serial.print(ch[j].pin);
    Serial.print("-> ");
    Serial.print(ain);
    if (ch[j].avg_cnt >1 ) {
      Serial.print(" Avg-> ");
      Serial.print(ch[j].asum/ch[j].avg_cnt); Serial.print(".");
      ch[j].asum = 1000 + (ch[j].asum % ch[j].avg_cnt)*1000;
      sprintf(str,"%4d",ch[j].asum/ch[j].avg_cnt);
      Serial.print(str+1); /* strip off the leading 1 to get frac */
    }
    Serial.println("");
    ch[j].cnt = 0;
    ch[j].asum = 0;
  }
  ch[j].lastval = ain;

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
