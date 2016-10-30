// AIO and WiFi configuration stuff

// The lines included below are authentication information and are not in GitHub
#include "C:\drm\PrivCode\WiFi_auth.h"
#include "C:\drm\PrivCode\Adafruit_IO_auth.h"

#include <SPI.h>
#include "AdafruitIO_WiFi.h"
// replaced by AdafuitIO_WiFi.h -- #include <WiFi101.h>

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include <AdafruitIO.h>
#include <AdafruitIO_Data.h>
#include <AdafruitIO_Definitions.h>

#define LED           13
#define BATT          A7
#define BAT_AVG_CNT   4

void printCurrentNet();
void printWifiData();
