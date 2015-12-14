/*
 * drm - EEPROM Read
 *
 * Reads the value of each byte of the EEPROM and prints it
 * to the computer.
 * This example code is in the public domain.
 * Modified by drm 20151212 by drm from IDE example sketch
 */

#include <EEPROM.h>
#define LINE_CNT 16

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;
char ascii[LINE_CNT+1];

void setup() {
  // initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  int i;
  for(i=0; i<LINE_CNT+1; i++) ascii[i]=NULL;
}

void loop() {
  
  // read a byte from the current address of the EEPROM
  value = EEPROM.read(address);
  ascii[address%LINE_CNT]=' ';
  if(value>=32 && value <=126)
  {
    ascii[address%LINE_CNT]=value;
  }

  if(address%LINE_CNT == 0)
  {
    Serial.print(address);
    Serial.print(" -");
  }
  Serial.print(" ");
  Serial.print(value, DEC);
  if((address+1)%LINE_CNT == 0)
  {
    Serial.println();
    Serial.println(ascii);
  }

  /***
    Advance to the next address, when at the end restart at the beginning.

    Larger AVR processors have larger EEPROM sizes, E.g:
    - Arduno Duemilanove: 512b EEPROM storage.
    - Arduino Uno:        1kb EEPROM storage.
    - Arduino Mega:       4kb EEPROM storage.

    Rather than hard-coding the length, you should use the pre-provided length function.
    This will make your code portable to all AVR processors.
  ***/
  address = address + 1;
  if (address == EEPROM.length()) 
  {
    address = 0;
  }

  /***
    As the EEPROM sizes are powers of two, wrapping (preventing overflow) of an
    EEPROM address is also doable by a bitwise and of the length - 1.

    ++address &= EEPROM.length() - 1;
  ***/

  delay(5);
}
