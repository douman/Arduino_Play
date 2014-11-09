#include <Wire.h>
#include <EEPROM.h>

const char *version="ChronoDot_20141027 -> V3.0.0-20141109   ";
const long msec_repeat=5000;
const int num_regs = 19;
const int DS3231_addr = 0x68; // DS3231 I2C address
unsigned long last_msec = 9999999; // initialize to weird value to assure quick first read
unsigned long last_sec=0;
unsigned long cycle_count=0;

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  DS3231_setup();
  drm_start_print();
}
 
void loop()
{
  char inbyte;
  byte read_by[num_regs];
//  Serial.print(millis()/1000); Serial.print(" - "); Serial.println(last_msec/1000);
  boolean next_sec = (last_msec/msec_repeat != millis()/msec_repeat);
  last_msec=millis();
  if (Serial.available() > 0) { // read command byte if available
    // get incoming character
    byte tempbyte = Serial.read(); 
    if(tempbyte != 13) { // ignore CR
      inbyte = tempbyte;
    }
    if(inbyte =='W') set_time(); // Write Clock if a W character is Sent - Time these are hardcoded
  }
  if(inbyte == 'R' || next_sec) { // Read Time
    s_prt_lead0(cycle_count, 8); Serial.print(" "); cycle_count++;    
    read_clock(read_by);
    if(inbyte == 'R') print_DS3231_registers(read_by); // print out registers, reset alarms and reguest temp
    print_time(read_by);    
  }
  inbyte=NULL;
}

short drm_serialno() {
  return(EEPROM.read(5) << 8 | EEPROM.read(6));
}

byte bcd2dec_byte(byte in_byte) {
  return (((in_byte & 0b11110000)>>4)*10 + (in_byte & 0b00001111));
}
void s_prt_lead0(long in, int places) {
  if(places>10 || places<2) return;
  in = abs(in); // only for positive numbers
  if(in < 1000000000) in = in + 1000000000; // extend smaller numbers
  char out_str[11];
  sprintf(out_str, "%ld", in);
  Serial.print((out_str+(10-places)));
  return;
}

void clear_alarms() {
  Wire.beginTransmission(DS3231_addr); // DS3231_addr is DS3231 device address
  Wire.write((byte)0x0f); // start at register 0x0F Status Register
  Wire.endTransmission();
  Wire.requestFrom(DS3231_addr, 1); // Read one byte only
  byte RTC_status = Wire.read();

  Wire.beginTransmission(DS3231_addr); // DS3231_addr is DS3231 device address
  Wire.write((byte)0x0f); // Status Register
  Wire.write((RTC_status & (byte) 0xFC)); // clear the alarm flags
  Wire.endTransmission();      
}  

void convert_temp() {
  Wire.beginTransmission(DS3231_addr); // DS3231_addr is DS3231 device address
  Wire.write((byte)0x0e); // start at register 0x0F Status Register
  Wire.endTransmission();
  Wire.requestFrom(DS3231_addr, 1); // Read one byte only
  byte RTC_cr = Wire.read();

  Wire.beginTransmission(DS3231_addr); // DS3231_addr is DS3231 device address
  Wire.write((byte)0x0e); // Status Register
  Wire.write((RTC_cr | (byte) 0x20)); // clear the alarm flags
  Wire.endTransmission();      
}

void DS3231_setup() {
  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  Wire.beginTransmission(DS3231_addr); // address DS3231
  Wire.write(0x0E); // select register
  Wire.write(0b00000110); // write register bitmap, bit 7 is /EOSC
                          // bit 2 - 0 are interupt/square wave/alarm enables, 
                          // #2 choses between sqwv and alarms
                          // #1 turns on alarm 2
                          // #0 turns on alarm 1
  Wire.write(0b00000000); // Clear the flags in the Status register
  Wire.endTransmission();

// Set up the Alarms
  Wire.beginTransmission(DS3231_addr); // address DS3231
  Wire.write(0x07); // select register -- Alarm registers
  Wire.write(0b10000000);
  Wire.write(0b10000000);
  Wire.write(0b10000000);
  Wire.write(0b10000000);
  Wire.write(0b10000000);
  Wire.write(0b10000000);
  Wire.write(0b10000000); // Alarm 1 every second Alarm 2 every minute
  Wire.endTransmission();

// Set up the Aging offset
  Wire.beginTransmission(DS3231_addr); // address DS3231
  Wire.write(0x10); // Aging offset select register -- Alarm registers
  Wire.write(0b00000000); // No Aging offset
  Wire.endTransmission();
}
void drm_start_print() {
  Serial.print(version); Serial.print(" - SN#");
  Serial.println(drm_serialno());
  Serial.print("Compiled-> ");
  Serial.print(__DATE__); 
  Serial.print(" -- ");
  Serial.println( __TIME__);
}

void print_time(byte *read_by) {
    // The below are all BCD encoded with some high control bits on some
    int seconds = read_by[0]; // get seconds
    int minutes = read_by[1]; // get minutes
    int hours = read_by[2];   // get hours
    int dow = read_by[3];   // get day of week (Mon = 0)
    int dom = read_by[4];   // get day of month
    int month = read_by[5];   // get month number (Jan = 1)
    int year = read_by[6];   // get year (last two digits)
    byte csr = read_by[14];
    byte sr = read_by[15];
    seconds = bcd2dec_byte(seconds);
    minutes = bcd2dec_byte(minutes);
    hours = bcd2dec_byte(0x3F & hours);
    dow = bcd2dec_byte(dow);
    dom = bcd2dec_byte(dom);
    month = bcd2dec_byte(0x1F & month);
    int int_year = 2000 + (100*(month>32)) + (long) bcd2dec_byte(year);
    unsigned long lsec = seconds + 60*(minutes + 60*(hours + 24*dom));
    
    s_prt_lead0(int_year,4); Serial.print("/");
    s_prt_lead0(month,2); Serial.print("/");
    s_prt_lead0(dom,2); Serial.print(" ");
    
    s_prt_lead0(hours,2); Serial.print(":");
    s_prt_lead0((long) minutes, 2); Serial.print(":");
    s_prt_lead0((long) seconds, 2);    
    Serial.print(" PST ");
    
    s_prt_lead0((long) int_year, 4);
    s_prt_lead0((long) month, 2);
    s_prt_lead0((long) dom, 2);
    s_prt_lead0((long) (hours & 0x3F), 2);
    s_prt_lead0((long) minutes, 2);
    s_prt_lead0((long) seconds, 2);
    
    Serial.print(" T-> ");
    Serial.print(read_by[17]);
    Serial.print(".");
    s_prt_lead0((long) (read_by[18] >> 6)*25, 2);
    Serial.print("C");
    long Ftemp = 3200 + ((read_by[17]*100 + (read_by[18] >> 6)*25)*9)/5;
    Serial.print("/");
    Serial.print(Ftemp/100);
    Serial.print(".");
    s_prt_lead0(Ftemp%100, 2);
    Serial.print("F");
    Serial.print(" dS-> ");
    Serial.print(lsec - last_sec); last_sec = lsec;
    if((sr & 0x04) != 0) Serial.print("  Bsy");
    if((sr & 0x01) != 0) Serial.print("  A0");
    if((sr & 0x02) != 0) Serial.print("  A1");
    Serial.println("");
}
void set_time() {
  Wire.beginTransmission(DS3231_addr); // DS3231_addr is DS3231 device address
//  Set Time
    Wire.write((byte)0); // start at register 0
    Wire.write((byte)0x00); // Seconds
    Wire.write((byte)0x24); // Minutes
    Wire.write((byte)0x07); // Hour register
//  Set Date
//      Wire.write((byte)3); // start at register 3
//      Wire.write((byte)0x01); // Day of Week
//      Wire.write((byte)0x28); // Day of Month
//      Wire.write((byte)0x10); // Month
//      Wire.write((byte)0x14); // Year
    Wire.endTransmission();
}
void read_clock(byte *read_by) {
    int i;
    Wire.beginTransmission(DS3231_addr); // DS3231_addr is DS3231 device address
    Wire.write((byte)0); // start at register 0
    Wire.endTransmission();
    Wire.requestFrom(DS3231_addr, num_regs); // request read of all registers
    for (i=0;(i<num_regs && Wire.available());i++) { read_by[i] = Wire.read(); }
    if (i!=num_regs) { Serial.print("Read only "); Serial.print(i); Serial.println(" bytes");}
}

void print_DS3231_registers(byte *read_by) {
      Serial.print("DigIn[5]-> "); Serial.println(digitalRead(5));
      for (int i=0;i<num_regs;i++) {if(read_by[i]<16)Serial.print("0");Serial.print(read_by[i],HEX); Serial.print(" ");}
      Serial.println(" ");
      clear_alarms();
      convert_temp();
    }
