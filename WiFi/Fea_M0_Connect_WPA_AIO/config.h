// AIO and WiFi configuration stuff

#include <SPI.h>
// replaced by AdafuitIO_WiFi.h -- 
#include <WiFi101.h>
#include "AdafruitIO_WiFi.h"

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include <AdafruitIO.h>
#include <AdafruitIO_Data.h>
#include <AdafruitIO_Definitions.h>

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

