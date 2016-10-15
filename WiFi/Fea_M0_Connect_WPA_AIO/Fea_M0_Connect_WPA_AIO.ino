/*
 * Derived from AdaFruit Examples and my own code base
 * 
 * Connection of Feather M0 to AdaFruit IO for logging using MQTT
 * written (synthesized) by drm 20160926
 */
 const char *code_version="Fea_M0_Connect_WPA_AIO -> V1.0-20160926 ";
#include <drmLib.h>

#include "config.h"

// AdafruitIO_WiFi aio(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_WiFi aio(AIO_USERNAME, AIO_KEY, WIFI_SSID,"");


#define LED           13
#define BATT          A7
#define BAT_AVG_CNT   4

// The lines included below are authentication information and are not in GitHub
#include "C:\drm\PrivCode\WiFi_auth.h"
#include "C:\drm\PrivCode\Adafruit_IO_auth.h"

//------ The items below are defined in the WiFi_auth.h include file
// #define WIFI_SSID       "your_ssid"
// #define WIFI_PASS       "your_pass"

//------ The items below are defined in the Adafruit_IO_auth.h include file
// #define AIO_USERNAME    "your_username"
// #define AIO_KEY         "your_key"

int status = WL_IDLE_STATUS;     // the Wifi radio's status
float batt_volts;
int serial_wrt_free;
long millis_start;
unsigned long last_run_secs = 0;
boolean serPrt = true;
// boolean serPrt = false;

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
    // if (pass == "")
      status = WiFi.begin(ssid);
    // else
      // status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

}

void loop() 
{
  // check the network connection once every 5 seconds:
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

