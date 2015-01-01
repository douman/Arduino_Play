/*
  command_parser
  With a watchdog 
  
  modified by drm 20141006 to learn Arduino
 */
#include <avr/wdt.h>
#include <EEPROM.h>

const int led = 13;
const unsigned int  cmd_sz=32;
const unsigned int eeprom_sz=32;
const char *version="command_parser -> V1.2.0-20141231";

// the setup routine runs once upon reset
void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);  
  watchdogSetup();
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  drm_Start_print();
}

// the loop routine runs over and over again forever:
char inbyte;
int i, idx;
char *cmd_ptr, cmd[cmd_sz];
byte istat;

void loop() {
  
  wdt_reset();
    
  if (Serial.available() > 0) {
    // get incoming byte:
     inbyte = Serial.read();
    if(inbyte == 10 || inbyte == 13 || inbyte == NULL) { // command terminators
      *(cmd+idx) = NULL;
      istat = execute_cmd(cmd);
      idx = 0;
      *cmd=NULL;
    }
    else if(idx < (cmd_sz-1)) *(cmd+idx++)=inbyte; // command must be less than cmd_sz - 1 long
  }
}


byte execute_cmd(char *cmd) {
  String cmd_ptr = String(cmd);
  Serial.print("Cmd-> "); Serial.println(cmd);
  if (cmd_ptr.equals("Reset")) {
    Serial.println("--Got Reset Command--");  
    Serial.print("RetVal-> "); Serial.println(ee_write(true, true));
  }
  else if (cmd_ptr.equals("?")) {
    Serial.println("\"Reset\" will write to EEPROM");
    Serial.println("Any other string will show EEPROM header contents");
  }
  drm_Start_print();
  for (i=0; i<eeprom_sz; i++) {
    inbyte = EEPROM.read(i);
    if(inbyte > 33 && inbyte < 128) Serial.print((char) inbyte);
    else Serial.print((unsigned int) inbyte); 
    Serial.print(" ");
  }
  Serial.println("");
}

// Write header into EEPROM
byte ee_write(char reset, char update) {
  char ret_val;
  if(reset) {
    if (EEPROM.read(0) != 123) EEPROM.write(0, 123);
    if (EEPROM.read(1) != 100) EEPROM.write(1, 100);
    if (EEPROM.read(2) != 114) EEPROM.write(2, 114);
    if (EEPROM.read(3) != 109) EEPROM.write(3, 109);
    if ((ret_val = EEPROM.read(4)) != 1)   EEPROM.write(4, 1);
    for (int m=5; m<eeprom_sz; m++) if (EEPROM.read(m) != 0) EEPROM.write(m, 0);
  }
  else if (update) EEPROM.write(4,ret_val=EEPROM.read(4)+1);
  return (0);
}

// Setup the CSR for the Watchdog Timer
void watchdogSetup(void)
{
  /*
  WDTCSR configuration:
  WDIE = 1: Interrupt Enable
  WDE = 1 :Reset Enable
  See table for time-out variations:
  WDP3 = 0, WDP2 = 1, WDP1 = 1, WDP0 = 1 :For 2000ms Time-out
  WDP3 = 0, WDP2 = 1, WDP1 = 1, WDP0 = 0 :For 1000ms Time-out
  WDP3 = 1, WDP2 = 0, WDP1 = 0, WDP0 = 0 :For 1000ms Time-out
  */
  
  cli();
  wdt_reset();
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
  sei();
}


void drm_Start_print() {
  Serial.print(version); Serial.print(F(" SN#"));
  Serial.println(drm_Serialno());
  Serial.print(F("Compiled-> "));
  Serial.print(F(__DATE__)); 
  Serial.print(F(" "));
  Serial.println(F(__TIME__));
}

unsigned short drm_Serialno() {
  return(EEPROM.read(5) << 8 | EEPROM.read(6)); // combine two bytes into in serial number (drm specific)
}
