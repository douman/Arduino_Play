/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * 
 *
 **************************************************************
 * 
 * 
 *
 * WARNING :
 * For this example you'll need SimpleTimer library:
 *   https://github.com/jfturcot/SimpleTimer
 * Visit this page for more information:
 *   http://playground.arduino.cc/Code/SimpleTimer
 *
 * 
 *   DHT22 ----gpio12
 *
 **************************************************************/

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>
#define DHTPIN 12 //pin gpio 12 in sensor
#define DHTTYPE DHT22   // DHT 22 Change this if you have a DHT11
DHT dht(DHTPIN, DHTTYPE);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "xxxxxxxxxx";  // Put your Auth Token here. (see Step 3 above)

SimpleTimer timer;

void setup()
{
  Serial.begin(9600); // See the connection status in Serial Monitor
   Blynk.begin(auth, "ssid", "password!"); //insert here your SSID and password
 
  // Setup a function to be called every second
  timer.setInterval(1000L, sendUptime);
}

void sendUptime()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
   //Read the Temp and Humidity from DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Blynk.virtualWrite(10, t); // virtual pin 
  Blynk.virtualWrite(11, h); // virtual pin 
}

void loop()
{
  Blynk.run();
  timer.run();
}
