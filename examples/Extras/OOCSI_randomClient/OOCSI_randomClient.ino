/****************************************************************************
   Example of the OOCSI-ESP library connecting to WiFi and OOCSI with a
   client name that is partly random, so the server will not reject this 
   client if it briefly disconnects.
 ****************************************************************************/

#include "OOCSI.h"

// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// name for connecting with OOCSI (unique handle)
// if you use # characters in the name, they will be replaced by random numbers
// so you can ensure that your ESP connects to the server
const char* OOCSIName = "ESP_OOCSI_TIME_#####";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();

// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);

  // output OOCSI activity on the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  oocsi.setActivityLEDPin(LED_BUILTIN);

  // use this to switch off logging to Serial
  // oocsi.setLogging(false);

  // setting up OOCSI. processOOCSI is the name of the fucntion to call when receiving messages, can be a random function name
  // connect wifi and OOCSI to the server
  oocsi.connect(OOCSIName, hostserver, ssid, password, processEvent);

  // channel subscriptions etc.
  // ...
}

void loop() {
  // put your main code here, to run repeatedly:
  // ...

  // let OOCSI check to process incoming data
  oocsi.check();
}

// function which OOCSI calls when an OOCSI message is received
void processEvent() {
  // use this to print out the raw message that was received
  // oocsi.printMessage();
}
