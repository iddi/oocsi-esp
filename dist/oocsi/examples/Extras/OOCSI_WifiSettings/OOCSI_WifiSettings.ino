/*************************************************************************************
   Example of the OOCSI-ESP library connecting to a Wifi network that can be
   interactively chosen using the WifiSettings library.

   Dependency: ESP-WifiSettings (you can install this via the Arduino Library manager)
   Source and further documentation available at
   https://github.com/Juerd/ESP-WiFiSettings
 *************************************************************************************/

#include <WiFiSettings.h>
#include <OOCSI.h>

// status LED
const uint32_t LED_PIN = 2;

// name for connecting with OOCSI (hashes will be randomized)
const char* OOCSIName = "client####";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(115200);

  // handle the Wifi setup and connection
  connectWithWifiSettings();

  // connect to OOCSI without the Wifi configuration and management
  oocsi.connect(OOCSIName, hostserver, processOOCSI);

  // subscribe to a channel
  Serial.println("Subscribing to testchannel");
  oocsi.subscribe("testchannel");
}

void loop() {
  // put your main code here, to run repeatedly:

  // let OOCSI check to process incoming data
  oocsi.check();
}

void processOOCSI() {
  Serial.println(oocsi.getInt("color", -1));
}




// -----------------------------------------------------------------------------------
// WifiSettings - Don't change code below
// -----------------------------------------------------------------------------------

#ifdef ESP32
#include <SPIFFS.h>
#else
#include <LittleFS.h>
#endif
#define LED_ON  LOW
#define LED_OFF HIGH

// handle Wifi setup and connection
void connectWithWifiSettings() {

  // Will format on the first run after failing to mount
#ifdef ESP32
  SPIFFS.begin(true);
#else
  LittleFS.begin();
#endif

  pinMode(LED_PIN, OUTPUT);

  // custom callback functions for LED progress indication
  WiFiSettings.onSuccess  = []() {
    // Turn LED on
    digitalWrite(LED_PIN, LED_ON);
  };
  WiFiSettings.onFailure  = []() {
    // Turn LED off
    digitalWrite(LED_PIN, LED_OFF);
  };
  WiFiSettings.onWaitLoop = []() {
    // Toggle LED
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    // Delay next function call by 500ms
    return 500;
  };

  // check the WifiSettings documentation / examples for more options

  // Connect to WiFi with a timeout of 20 seconds
  // Launches the portal if the connection failed
  WiFiSettings.connect(true, 20);
}
