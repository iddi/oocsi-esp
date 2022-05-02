/****************************************************************************
   Example of the OOCSI-ESP library connecting to WiFi and receiving messages
   over OOCSI. Designed to work with the Processing OOCSI DataSender example
 ****************************************************************************/

#include "OOCSI.h"

// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_OOCSI_CLIENT_MC_RECEIVER";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";
// name of the channel you want to subscribe to (can subscribe to multiple channels)
const char* channel1 = "esp-testchannel1";
const char* channel2 = "esp-testchannel2";
const char* channel3 = "esp-testchannel3";

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
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);

  // subscribe to a channel
  Serial.println("subscribing");
  oocsi.subscribe(channel1);
  oocsi.subscribe(channel2);
  oocsi.subscribe(channel3);
}

void loop() {
  // put your main code here, to run repeatedly

  // let OOCSI check to process incoming data
  oocsi.check();
}

// function which OOCSI calls when an OOCSI message is received
void processOOCSI() {
  if (oocsi.getRecipient() == channel1) {
    Serial.print("A message for channel1 from sender '");
    Serial.print(oocsi.getSender());
    Serial.print("' at ");
    Serial.print(oocsi.getTimeStamp());
    Serial.println();
  } else if (oocsi.getRecipient() == channel2) {
    Serial.print("A message for channel2 from sender '");
    Serial.print(oocsi.getSender());
    Serial.print("' at ");
    Serial.print(oocsi.getTimeStamp());
    Serial.println();
  } else if (oocsi.getRecipient() == channel3) {
    Serial.print("A message for channel3 from sender '");
    Serial.print(oocsi.getSender());
    Serial.print("' at ");
    Serial.print(oocsi.getTimeStamp());
    Serial.println();
  } else if (oocsi.getRecipient() == OOCSIName) {
    Serial.print("A direct message for just me from sender '");
    Serial.print(oocsi.getSender());
    Serial.print("' at ");
    Serial.print(oocsi.getTimeStamp());
    Serial.println();
  }
}
