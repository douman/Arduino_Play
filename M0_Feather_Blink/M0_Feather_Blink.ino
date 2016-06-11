#include <Adafruit_GPS.h>

// #include <Adafruit_BLE.h>
// #include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BluefruitLE_UART.h>

// #include <SPI.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
#define BATT A7
#define GPS_BAUD 115200 // 9600 is factory, 57600 is faster
#define BAT_AVG_CNT 4

Adafruit_GPS myGPS(&Serial1);
byte cksum, savecksum;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // Serial1.begin(GPS_BAUD);
  Serial1.begin(9600);
  myGPS.begin(GPS_BAUD);
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(A6, INPUT); // Battery level adc input
}

// the loop function runs over and over again forever
void loop() 
{
  // Serial.println(analogRead(BATT));
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(0);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(0);              // wait for a second
  // Serial.println("Reg Serial.println");
  // SerialUSB.println("USB Serial.println");

  if (Serial.available()) 
  {
    char c = Serial.read();
    Serial1.write(c);
  }
  if (Serial1.available()) 
  {
    char c = Serial1.read();
    
    if (c=='$')
    {
      // Get and print Battery voltage at beginning of GPS sentence
      float val=0;
      int i;
      for(i=0; i<BAT_AVG_CNT; i++) val += analogRead(BATT);
      val = val/(float)BAT_AVG_CNT;
      val = (val * (2*3.3))/1024;
      Serial.print("Chsum-> ");
      Serial.print(savecksum, HEX);
      Serial.print(" Batt-> ");
      Serial.println(val);

      // Start for Calculating sentence checksum
      cksum = 0;
    }
    if (c!='*')
    {
      cksum = cksum ^ c; // xor for checksum calculation
    }
    else
    {
      savecksum = cksum;
    }
    Serial.write(c);
  }
}
