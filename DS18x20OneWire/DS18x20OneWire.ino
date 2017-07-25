const char *code_version="DS18x20OneWire -> V1.04-20170331";
#include <OneWire.h>
#include <drmLib.h>

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
#define PIRIN A0

OneWire  ds(DATAPIN);  // on pin DATAPIN (a 4.7K resistor is necessary)
unsigned long icnt;


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
  pinMode     (PIRIN, INPUT);
  analogReference(INTERNAL);

  // pinMode(5, INPUT);
  drmStartPrint(code_version);
}

//------------------
void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  Serial.print(icnt++); Serial.print("- ");
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    //Serial.println();
    ds.reset_search();
    digitalWrite(LASER, HIGH);
    delay(250);
    digitalWrite(LASER, LOW);
    return;
  }
  
  Serial.print(" R=");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    //Serial.print(addr[i], HEX);
    drmPrtLead0Hex(addr[i]);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  //Serial.println();
 
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
  
  digitalWrite(LASER, HIGH);
  delay(100);
  int pirvolts = analogRead(PIRIN);
//  Serial.print(" PIR-> "); Serial.print(pirvolts); Serial.print(" ");
  if(pirvolts > 256) digitalWrite(LASER, LOW);
  delay(800);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print(" Data= ");
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
  Serial.print(" Temp= ");
  Serial.print(celsius);
  Serial.print(" C, ");
  Serial.print(fahrenheit);
  Serial.println(" F");
}
