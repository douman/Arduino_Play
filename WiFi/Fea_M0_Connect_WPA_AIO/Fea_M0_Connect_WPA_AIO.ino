/*
 * Derived from AdaFruit Examples and my own code base
 * 
 * Connection of Feather M0 to AdaFruit IO for logging using MQTT
 * written (synthesized) by drm 20160926
 */
const char *code_version="Fea_M0_Connect_WPA_AIO -> V1.0-20160926 ";
#include <drmLib.h>

#include "config.h"

//------ The items below are defined in the WiFi_auth.h include file
// WIFI_SSID       WIFI_PASS       
//------ The items below are defined in the Adafruit_IO_auth.h include file
// AIO_USERNAME    AIO_KEY

AdafruitIO_WiFi aio(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS);
// Set up the AIO feeds
AdafruitIO_Feed *volts = aio.feed("volts");
AdafruitIO_Feed *secs = aio.feed("secs");
AdafruitIO_Feed *sout_buf = aio.feed("sout_buf");

int status = WL_IDLE_STATUS;     // the Wifi radio's status
float batt_volts;
int serial_wrt_free;
long millis_start;
unsigned long last_run_secs = 0;
boolean serPrt = false; // true;

void setup() 
{
  millis_start = millis();
  WiFi.setPins(8,7,4,2);  // Configure Feather M0 WiFi Pins
  //Initialize serial and wait for port to open:

  // initialize digital pin (LED) 13 as an output and the battery level as analog input
  analogReadResolution(12);
  pinMode(LED, OUTPUT); digitalWrite(LED, LOW);
  pinMode(BATT, INPUT); // Battery level adc input
  
  Serial.begin(115200);
  delay(1000);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  drmStartPrint(code_version);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) 
  {
    Serial.println("WiFi shield not present-spinning");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  Serial.print("Firmware Version-> "); Serial.println(fv);
  if (fv != "19.4.4") 
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    if (pass == "")
      status = WiFi.begin(ssid);
    else
      status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  aio.connect();

  // wait for a connection
  while(aio.status() < AIO_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
}

void loop() 
{
  if(Serial.available())
  {
    char c = Serial.read();
    if(c == 'S' || c == 's') 
    { 
      serPrt = !serPrt; 
      Serial.print("# SER swap "); 
      if(serPrt) Serial.println(serPrt); 
    }
  }
  int delay_sec = 15;
  if((millis()-millis_start)/1000 > 120) delay_sec = 300;
  if(last_run_secs + delay_sec < (millis()-millis_start)/1000) 
  {
    aio.run();
    last_run_secs = (millis()-millis_start)/1000;
    printCurrentNet();
    printWifiData();    
    secs->save(last_run_secs);
    volts->save(batt_volts);
    sout_buf->save(serial_wrt_free);
  }
  // WiFi.maxLowPowerMode();
  WiFi.lowPowerMode();
}

void printWifiData() 
{
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  if(serPrt) Serial.print("IP Address: ");
  if(serPrt) Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  int i;
  WiFi.macAddress(mac);
  if(serPrt) Serial.print("MAC address: ");
  for(i=5; i>=0; i--)
  {
    if(serPrt) Serial.print(mac[i], HEX);
    if (i!=0) if(serPrt) Serial.print(":");      
  }
}

void printCurrentNet() 
{
  // print the SSID of the network you're attached to:
  if(serPrt) Serial.println();
  if(serPrt) Serial.print("SSID: ");
  if(serPrt) Serial.println(WiFi.SSID());

  // Print run time
  if(serPrt) Serial.print("Running: ");
  if(serPrt) Serial.println((millis() - millis_start)/1000);
  
  // Get voltage, store in global and print
  if(serPrt) Serial.print("Volts: ");
  if(serPrt) Serial.println(batt_volts = read_batt());

  // Get serial buffer free, store in global and print
  if(serPrt) Serial.print("WriteFree: ");
  if(serPrt) Serial.println(serial_wrt_free = Serial.availableForWrite());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  if(serPrt) Serial.print("BSSID: ");
  if(serPrt) Serial.print(bssid[5], HEX);
  if(serPrt) Serial.print(":");
  if(serPrt) Serial.print(bssid[4], HEX);
  if(serPrt) Serial.print(":");
  if(serPrt) Serial.print(bssid[3], HEX);
  if(serPrt) Serial.print(":");
  if(serPrt) Serial.print(bssid[2], HEX);
  if(serPrt) Serial.print(":");
  if(serPrt) Serial.print(bssid[1], HEX);
  if(serPrt) Serial.print(":");
  if(serPrt) Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  if(serPrt) Serial.print("signal strength (RSSI):");
  if(serPrt) Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  if(serPrt) Serial.print("Encryption Type:");
  if(serPrt) Serial.println(encryption, HEX);
  if(serPrt) Serial.println();
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

