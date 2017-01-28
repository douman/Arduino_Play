/*
 * Derived from AdaFruit Examples and my own code base
 * 
 * Connection of Feather M0 to AdaFruit IO for logging using MQTT
 * written (synthesized) by drm 20160926
 * V1.1 Made times defined parameters
 * V1.2 extra low power mode appears to cause problems, trying to get timing right
 * V1.3 some unknown change
 * V1.4 changed 'S' command behavior, now will print after s command
 */

const char *code_version="Fea_M0_Connect_WPA_AIO -> V1.4-20170126 ";
#include <drmLib.h>

#include "config.h"

//------ The items below are defined in the WiFi_auth.h include file
// WIFI_SSID       WIFI_PASS       
//------ The items below are defined in the Adafruit_IO_auth.h include file
// AIO_USERNAME    AIO_KEY

AdafruitIO_WiFi aio(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS);
// Set up the AIO feeds
AdafruitIO_Feed *volts = aio.feed("vb1");
AdafruitIO_Feed *secs = aio.feed("sb1");
// AdafruitIO_Feed *volts = aio.feed("voltsgrn2");
// AdafruitIO_Feed *secs = aio.feed("secsgrn2");
// AdafruitIO_Feed *volts = aio.feed("vr3");
// AdafruitIO_Feed *secs = aio.feed("sr3");

int status = WL_IDLE_STATUS;     // the Wifi radio's status
float batt_volts;
unsigned long millis_start;
unsigned int loop_run_time = 0;
unsigned long last_run_secs = 0;
boolean serPrt = true, printNext=false;
// boolean serPrt = false;

void setup()
{
  millis_start = millis();

  // initialize digital pin (LED) 13 as an output and the battery level as analog input
  analogReadResolution(12);
  pinMode(LED, OUTPUT); digitalWrite(LED, LOW);
  pinMode(BATT, INPUT); // Battery level adc input
  
  Serial.begin(115200);
  delay(3000);
  drmStartPrint(code_version);

  WiFiConnect();

  if(serPrt) Serial.print("Connecting to Adafruit IO");


  // connect to io.adafruit.com
  aio.connect();

  // wait for a connection
  while(aio.status() < AIO_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  if(serPrt) Serial.println();
  if(serPrt) Serial.print("AdaFruit IO Connection Sucess-> ");
  if(serPrt) Serial.println(aio.statusText());
}

void loop() 
{
  loop_run_time = millis();
  if(Serial.available())
  {
    char c = Serial.read();
    if(c == 'S' || c == 's') 
    { 
      serPrt = !serPrt;
      printNext = !printNext;
      Serial.print("# SER swap "); 
      Serial.println(serPrt); 
    }
  }
  if(WiFi.status() != WL_CONNECTED)   WiFiConnect();
  int delay_sec = INITIAL_INTERVAL_SECS;
  if((millis()-millis_start)/1000 > INITIAL_SECS) delay_sec = INTERVAL_SECS;
  if((last_run_secs + delay_sec < (millis()-millis_start)/1000) || printNext)
  {
    printNext = false;
    digitalWrite(LED, HIGH);
    last_run_secs = (loop_run_time-millis_start)/1000;
    aio.run();
    
    printCurrentNet();
    printWifiData();    
    secs->save(last_run_secs);
    volts->save(batt_volts);
    digitalWrite(LED, LOW);
  }
  WiFi.lowPowerMode(); // Appears to cause disconnect problems
  // WiFi.maxLowPowerMode(); // Appears to cause disconnect problems
  loop_run_time = millis();

}

void printCurrentNet() 
{
//  // print the SSID of the network you're attached to:
//  if(serPrt) Serial.println();
//  if(serPrt) Serial.println();
//  if(serPrt) Serial.print("SSID: ");
//  if(serPrt) Serial.println(WiFi.SSID());
//
//  // Print run time
//  if(serPrt) Serial.print("Running: ");
//  if(serPrt) Serial.println((millis() - millis_start)/1000);
//  
  // Get voltage, store in global and print
  if(serPrt) Serial.print("Volts: ");
  batt_volts = read_batt();
  byte bssid[6];
  WiFi.BSSID(bssid);

  // print the received signal strength and encryption
  long rssi = WiFi.RSSI();
  if(serPrt)
  {
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);
  }

  if(serPrt)
  {
    // print the SSID of the network you're attached to:
    Serial.println();
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
  
    // Print run time
    Serial.print("Running: ");
    Serial.println((millis() - millis_start)/1000);
    
    // Get voltage, store in global and print
    Serial.print("Volts: ");
    Serial.println(batt_volts);
  
    // print the MAC address of the router you're attached to:
    int i;
    Serial.print("BSSID: ");
    for(i=5; i>=0; i--)
    {
      Serial.print(bssid[i], HEX);
      if (i!=0) Serial.print(":");
      else Serial.println();
    }
  
    // print the received signal strength:
    Serial.print("signal strength (RSSI): ");
    Serial.println(rssi);
  
    // print the encryption type:
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type: ");
    Serial.println(encryption, HEX);
    Serial.print("Status: ");
    Serial.println(WiFi.status());
  }
}

void printWifiData()
{
  // get data from WIFi
  byte mac[6];
  WiFi.macAddress(mac);
  IPAddress ip = WiFi.localIP();
  unsigned long unixtime = WiFi.getTime();
  unsigned long unixyear = unixtime/(60*60*24);
  unixyear = ((float) unixyear)/365.2422; // from JPL web paper by Evan M Manning
  
  // print your WiFi shield's IP and MAC addresses
  if(serPrt)
  {
    Serial.print("Unix Time: ");
    Serial.println(unixtime);
    Serial.print("Unix year: ");
    Serial.println(unixyear);
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.print("MAC address: ");
    int i;
    for(i=5; i>=0; i--)
    {
      Serial.print(mac[i], HEX);
      if (i!=0) Serial.print(":");      
      else Serial.println();      
    }
  }
}


float read_batt()
{
  // Get and print Battery voltage at beginning of GPS sentence
  float val=0;
  int i;
  for(i=0; i<BAT_AVG_CNT; i++) val += analogRead(BATT);
  val = val/(float)BAT_AVG_CNT;
  // val = (val * (2*3.3))/1024; // at lower ADC resolution (10 bit)
  val = (val * (2*3.3))/4096; // 12 bit ADC resolution
  return(val);
}

void WiFiConnect()
{
  WiFi.setPins(8,7,4,2);  // Configure Feather M0 WiFi Pins
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) 
  {
    if(serPrt) Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if(serPrt) Serial.print("Firmware Version-> "); if(serPrt) Serial.println(fv);
  if (fv != "19.4.4") 
  {
    if(serPrt) Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  WiFi.disconnect();
  delay(3000);
  while (status != WL_CONNECTED) 
  {
    if(serPrt) Serial.print("Attempting to connect to WPA SSID: ");
    if(serPrt) Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 3 seconds for connection:
    delay(3000);
  }
  // you're connected now
  if(serPrt) Serial.print("You're connected to the network");
}
