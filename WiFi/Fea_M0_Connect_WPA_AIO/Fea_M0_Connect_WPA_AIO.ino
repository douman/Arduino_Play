/*
 * Derived from AdaFruit Examples and my own code base
 * 
 * Connection of Feather M0 to AdaFruit IO for logging using MQTT
 * written (synthesized) by drm 20160926
 */
 const char *code_version="Fea_M0_Connect_WPA_AIO -> V1.0-20160926 ";
#include <drmLib.h>

#include "config.h"

AdafruitIO_WiFi aio(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS);

// Set up the AIO feeds
AdafruitIO_Feed *volts = aio.feed("volts");
AdafruitIO_Feed *secs = aio.feed("secs");
AdafruitIO_Feed *sout_buf = aio.feed("sout_buf");

int status = WL_IDLE_STATUS;     // the Wifi radio's status
float batt_volts;
int serial_wrt_free;
long millis_start;

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
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
    drmStartPrint(code_version);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) 
  {
    Serial.println("WiFi shield not present");
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
  while(aio.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.print("AdaFruit IO Connection Sucess-> ");
  Serial.println(aio.statusText());

  // WiFi.setSleepMode(M2M_PS_H_AUTOMATIC, 1); // go into power save mode when possible! 
  // WiFi.setSleepMode(M2M_PS_MANUAL, 1);
}

void loop() 
{
  aio.run();
  volts->save(batt_volts);
  secs->save((millis()-millis_start)/1000);
  sout_buf->save(serial_wrt_free);
 
  // check the network connection once every 5 seconds:
  delay(15000);
  printCurrentNet();
  printWifiData();
}

void printWifiData() 
{
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  int i;
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  for(i=5; i>=0; i--)
  {
    Serial.print(mac[i], HEX);
    if (i!=0) Serial.print(":");      
  }
}

void printCurrentNet() 
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
  Serial.println(batt_volts = read_batt());

  // Get serial buffer free, store in global and print
  Serial.print("WriteFree: ");
  Serial.println(serial_wrt_free = Serial.availableForWrite());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
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

