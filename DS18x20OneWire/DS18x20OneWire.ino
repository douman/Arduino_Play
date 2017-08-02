const char *code_version="DS18x20OneWire -> V1.04-20170331";
#include <OneWire.h>
#include <drmLib.h>
#include "DS18x20OneWire.h"

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

#define POWERPIN 5
#define GNDPIN 7
#define DATAPIN 6
#define LASER 12
#define PIR_IN 2
#define ALARM_MS 5000

OneWire  ds(DATAPIN);  // on pin DATAPIN (a 4.7K resistor is necessary)
unsigned long icnt, laser_ontime = 0, motion_cnt=0;
boolean laser_state = false;


//-------------------
void setup(void) {
  icnt = 0;
  Serial.begin(115200);
  
  pinMode     (GNDPIN, OUTPUT);
  digitalWrite(GNDPIN, LOW);
  pinMode     (POWERPIN, OUTPUT);
  digitalWrite(POWERPIN, HIGH);
  pinMode     (DATAPIN, INPUT_PULLUP);
  pinMode     (LASER, OUTPUT);
  digitalWrite(LASER, LOW);
  laser_state = false;
  pinMode     (PIR_IN, INPUT);
  analogReference(INTERNAL);

  // pinMode(5, INPUT);
  drmStartPrint(code_version);
  
  byte addr[8];
  while (ds.search(addr))
  {
    ds.reset();
    ds.select(addr);
    ds.write(0x4E, 1); // write scratchpad command
    ds.write(0x00, 1); // alarm low limit
    ds.write(0x00, 1); // alarm high limit
    ds.write(0x7f, 1); // config register (set for 12 bit measurments (750 mSec)
    ds.reset();
    ds.select(addr);
    ds.write(0x48, 1); // write scratchpad to EEPROM
  }
  ds.reset_search();

}

//------------------
void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  int istat=0;
  float celsius, fahrenheit;
  Serial.print(icnt++); Serial.print("- ");

//  istat = triggerAllDS(ds);
//  delay(1800); // wait for conversions
  
  if ( !ds.search(addr)) 
  {
    Serial.println("No more addresses.");
    ds.reset_search();
    return;
  }
  
  Serial.print("R=");
  for( i = 0; i < 8; i++) 
  {
    Serial.write(' ');
    drmPrtLead0Hex(addr[i]);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) 
  {
      Serial.println("CRC is not valid!");
      return;
  }
  
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.print(" DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.print(" DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.print(" DS1822");
      type_s = 0;
      break;
    default:
      Serial.print("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  delay(800);
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print(" D= ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    drmPrtLead0Hex(data[i]);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  drmPrtLead0Hex(OneWire::crc8(data, 8));
  Serial.print(" --");

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print(" T=");
  Serial.print(celsius);
  Serial.print(" C ");
  Serial.print(fahrenheit);
  Serial.print(" F ");
  unsigned long mstime = millis();
  Serial.print(mstime/60000);
  Serial.print(":");
  Serial.println((mstime/1000) % 60);
  

  boolean pirstate = digitalRead(PIR_IN);
  unsigned long nowtime = millis();
  if (pirstate)
  {
    laser_ontime = nowtime;
    if (!laser_state)
    {
      Serial.print(icnt++); Serial.print("- ");
      Serial.print("Motion - "); Serial.println(motion_cnt);
      digitalWrite(LASER, HIGH);
      laser_state = true;
    }
  }
  long time_diff = laser_ontime - nowtime;
  if(abs(time_diff) > ALARM_MS && laser_state) 
  {
    Serial.print(icnt++); Serial.print("- ");
    Serial.print("Motion Cleared "); Serial.println(motion_cnt++);
    digitalWrite(LASER, LOW);
    laser_state = false;
  }
}
