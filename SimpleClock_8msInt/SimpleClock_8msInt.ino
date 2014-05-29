//#define GPS_SERIAL_SOFT
//#define ETHERNET false
#include <Timer1.h>  // https://github.com/carlosrafaelgn/ArduinoTimer
//#include <SPI.h>         // needed for Arduino versions later than 0018
//#include <Ethernet.h>
//#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
//#include <Wire.h> //I2C needed for sensors
//#include "MPL3115A2.h" //Pressure sensor
//#include "HTU21D.h" //Humidity sensor
#include <MemoryFree.h>

//#ifdef GPS_SERIAL_SOFT
//  #include <SoftwareSerial.h> //Needed for GPS
//#endif

//#include <TinyGPS++.h> //GPS parsing

//#define SENSOR_STRING "Meas_num\tHumidity\tTempC\tPressure\tBatt_lvl\tLight_lvl\tpAlt\tgAlt\tLat\tLon\tgAge\t#Sats\tVSent\tDate\tTime\n"

#define T1_INTERVAL (byte)8   // timer interval in msec.  //  125 interupts per second 

#define INT40ms 4   //  'A' 32 msec. ( 5 T1_INTERVAL count interupt values)
#define INT0 0      //  'B' 000 msec.  These run once per second at these offsets
#define INT2 25     //  'C' 200 msec.  These run once per second at these offsets
#define INT4 50     //  'D' 400 msec.
#define INT6 75     //  'E' 600 msec.
#define INT8 100    //  'F' 800 msec.

//#define PKT_SIZE 140

  // time structure varible
struct Time  {
  byte hours;
  byte minutes;
  unsigned int seconds2;
  byte seconds;
  byte msecs8;
  byte msecs8sec;  
  char timestring[16];
};
struct Time mytime, start;

//MPL3115A2 myPressure; //Create an instance of the pressure sensor
//HTU21D myHumidity; //Create an instance of the humidity sensor
//TinyGPSPlus gps;

//#ifdef GPS_SERIAL_SOFT
//   static const int RXPin = 5, TXPin = 9; //GPS is attached to pin 9(TX from GPS) and pin 5(RX into GPS)
//   SoftwareSerial ss(RXPin, TXPin); 
//#endif

const byte RED = 3;
const byte GREEN = 5;
const byte BLUE = 6;

const byte STAT1 = 7;  // digital I/O pins
const byte STAT2 = 8;
const byte GPS_PWRCTL = 6; //Pulling this pin low puts GPS to sleep but maintains RTC and RAM

//const byte REFERENCE_3V3 = A3;  // analog I/O pins
//const byte LIGHT = A1;
//const byte BATT = A2;
//const byte OPEN_ADC0 = A0; // WDIR = A0;
boolean int40=false, int000=false, int200=false, int400=false, int600=false, int800=false, intmod=false, times_up=false;

// ksm globals
//char fbuf[13];  //floating point string buffer for use with dtostrf()
char line_buff[120];  // buffer for line of data to send to serial, SD, ether
//unsigned int meas_num=0;
//byte CurrentSeconds = 0;  //
//unsigned long int ElapsedSeconds = 0;
unsigned long int tbegin, total_int_time = 0;

//float humidity = 0; // [%]
//float tempC = 0; // [temperature C]
//float pressure = 0;
//float batt_lvl = 0; //[analog value from 0 to 1023]
//float light_lvl = 1023; //[analog value from 0 to 1023]

//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  original
//byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xE3, 0x08 };  // back of ether#1
// byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xE2, 0x4B };  // back of ether#2
//IPAddress ip(192, 168, 0, 33);   // Local IP address of Arduino
//IPAddress macIP_Addr(192, 168, 0, 65);  // Remote computer IP address
//IPAddress macIP_Addr(255, 255, 255, 255);  // UDP Broadcast IP address
//const unsigned int localPort = 61557;      // local port to listen on
//const unsigned int RemotePort = 61556;      // Remote port to Send to
//EthernetUDP Udp;  // An EthernetUDP instance to let us send and receive packets over UDP

void setup()
{
  Serial.begin(115200);   
  
  Serial.println("SimpleClock_8msInt");
  start.hours =13;
  start.minutes = 40;
  start.seconds = 0;

  pinMode(RED, OUTPUT); //Status LED Blue
  pinMode(GREEN, OUTPUT); //Status LED Green  
  pinMode(BLUE, OUTPUT); //Status LED Green  
  digitalWrite(GREEN, HIGH); 
//  pinMode(GPS_PWRCTL, OUTPUT);  // GPS power control - low puts GPS to sleep
//  digitalWrite(GPS_PWRCTL, HIGH);  // GPS ON
//  digitalWrite(GPS_PWRCTL, LOW);   // GPS OFF
  //Configure the pressure sensor
//  myPressure.begin(); // Get sensor online
//  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
//  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
//  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags 

  //Configure the humidity sensor
//  myHumidity.begin();
  
  
//#ifdef GPS_SERIAL_SOFT
//   ss.begin(9600); //Begin listening to GPS over software serial at 9600   
//#endif  
   
  // start the Ethernet and UDP:
//  Ethernet.begin( mac, ip);
//  Udp.begin(localPort);  
  
  // Prepare Timer1 update clock and check for events every T1_INTERVAL msec.
  startTimer1((uint32_t)T1_INTERVAL * (uint32_t)1000);
  
  Serial.println("Program SimpleClock_8msINnt");
  Serial.print("Timer1 Interupt setup at " );                       
  Serial.print(T1_INTERVAL);      
  Serial.println(" msec. Interval");        

  Serial.println("Timer/Clock online!");
  Serial.println("Letters are printed when 80 chars are reached, each leter represents a 200msec interupt");
  Serial.println("'n' lets you know what time it is and how much memory is free");
  Serial.println("'s' sets the start time to now so the time of day is accurate (ex s 15:31:03)");
  line_buff[0] = NULL;
//  strcat( line_buff, SENSOR_STRING);    
//  Serial.print( line_buff);  
//  Udp.beginPacket( macIP_Addr, RemotePort);
//  Udp.write( line_buff);  // send current buffer 
//  Udp.endPacket();  
  
  Serial.println("Exiting Setup");
}

void loop()   {   

//#ifdef GPS_SERIAL_SOFT
//  while (Serial.available())  gps.encode(Serial.read());  // Read and process GPS serial data as availible
//#endif   

  if( intmod == true) {
    intmod = false;
//    strcat(line_buff, "A");    
    }
    
  if( int000 == true) {
    int000 = false;
    strcat(line_buff, "B");    
    digitalWrite(RED, LOW);  
    }
    
  if( int200 == true) {
    int200 = false;
    strcat(line_buff, "C"); 
    digitalWrite(BLUE, LOW);     
    }

  if( int400 == true) {
    int400 = false;
    strcat(line_buff, "D");
    digitalWrite(RED, HIGH);     
    } 
 
   if( int600 == true) {
    int600 = false;
    strcat(line_buff, "E");
    digitalWrite(BLUE, HIGH);    
    }   
    
   if( int800 == true) {
    int800 = false;
    strcat(line_buff, "F");   
    }       
    
  if( strlen( line_buff) > 80)  {
   Serial.println( line_buff);
   line_buff[0] = NULL;
   }      
    
    if( Serial.available())  {
      if( Serial.peek() == 'n')  {
        Serial.read();   // remove from buffer
        Serial.print("freeMemory()=");
        Serial.println(freeMemory());
        print_time(); 
        }
      }
      
    if( Serial.available())  {
      if( Serial.peek() == 's')  {
        Serial.read();   // remove from buffer
        start.hours = Serial.parseInt(); 
        start.minutes = Serial.parseInt(); 
        start.seconds = Serial.parseInt(); 

        mytime.msecs8sec = 0;
        mytime.msecs8 = 0;
        mytime.seconds2 =0;       
        
        Serial.print("Set time command - TZero set at ");
        Serial.print( start.hours);
        Serial.print(':');
        Serial.print( start.minutes);
        Serial.print(':');
        Serial.println( start.seconds); 
        }
      }      
      
  }


// Define the function which will handle the notifications
ISR(timer1Event)
{
  // Reset Timer1 (resetTimer1 should be the first operation for better timer precision)
//  resetTimer1();
  resetTimer1Unsafe();  
  tbegin = micros();
  // For a smaller and faster code, the line above could safely be replaced with a call
  // to the function resetTimer1Unsafe() as, despite of its name, it IS safe to call
  // that function in here (interrupts are disabled)

  // Make sure to do your work as fast as possible, since interrupts are automatically
  // disabled when this event happens (refer to interrupts() and noInterrupts() for
  // more information on that)
  
  switch ( mytime.msecs8sec)  {

    case INT0:
      int000 = true;
      break;    
      
    case INT2:
      int200 = true;
      break;      

    case INT4:
      int400 = true;      
      break; 
      
    case INT6:
      int600 = true;      
      break;       
      
    case INT8:
      int800 = true;      
      break;
  }
   
  if( mytime.msecs8 % INT40ms == 0)  {
    intmod = true;
    }
    
  // Update clock registers    
  ++mytime.msecs8;
  ++mytime.msecs8sec;
  
  if( mytime.msecs8sec >= 125)  {
    mytime.msecs8sec = 0;
    if( mytime.msecs8 >= 250)  {
      mytime.msecs8 = 0;
      ++mytime.seconds2;
      }      
    }
    total_int_time += micros() - tbegin;
}

void print_time()  {  // Print disabled for HW GPS
    unsigned int residual;
    long lseconds2;

    total_int_time = 0;
    mytime.hours = mytime.seconds2 / 1800;
    residual = mytime.seconds2 % 1800;
    mytime.minutes = residual / 30;
    mytime.seconds = (residual % 30) << 1;
    if( mytime.msecs8 > 124)   ++mytime.seconds;
    
    sprintf( mytime.timestring, "%02d:%02d:%02d.%03d", mytime.hours, mytime.minutes, mytime.seconds, (int)mytime.msecs8sec << 3);  
    Serial.println( mytime.timestring);
    
    lseconds2 = ((long)start.seconds >> 1) + ((long)start.minutes * 30) + ((long)start.hours * 1800);
    lseconds2 += mytime.seconds2;
    mytime.hours = lseconds2 / 1800;
    residual = lseconds2 % 1800;
    mytime.minutes = residual / 30;
    mytime.seconds = (residual % 30) << 1;
    if( mytime.msecs8 > 124)   ++mytime.seconds;
    
    Serial.print( mytime.msecs8sec);
    Serial.print(" : 8msec (1sec)\t");
    Serial.print( mytime.msecs8);
    Serial.print(" : 8msec (2sec)\t");
    Serial.print( mytime.seconds2);
    Serial.print(" : 2Seconds\t");
    Serial.print( total_int_time);
    Serial.println(" : Total interupt time");    
 
    sprintf( mytime.timestring, "%02d:%02d:%02d.%03d", mytime.hours, mytime.minutes, mytime.seconds, (int)mytime.msecs8sec << 3);  
    Serial.println( mytime.timestring); 
}

char* timestring()  {
    sprintf( mytime.timestring, "\t%02d:%02d:%02d.%03d\n", mytime.hours, mytime.minutes, mytime.seconds, (int)mytime.msecs8<<3);  
    return( mytime.timestring);
}



