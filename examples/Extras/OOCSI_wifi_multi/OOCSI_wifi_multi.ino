/*************************************************************************************
   Example of the OOCSI-ESP library connecting to a Wifi network that can be
   interactively chosen using the WifiMulti functionality.
 *************************************************************************************/

#include <WiFi.h>
#include <WiFiMulti.h>
#include <OOCSI.h>

WiFiMulti wifiMulti;

// name for connecting with OOCSI (hashes will be randomized)
const char* OOCSIName = "client####";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();

void setup()
{
  Serial.begin(115200);
  delay(10);

  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting any available Wifi that configured ...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // now connect to OOCSI without the Wifi configuration and management
    oocsi.connect(OOCSIName, hostserver, processOOCSI);

    // subscribe to a channel
    Serial.println("Subscribing to testchannel");
    oocsi.subscribe("testchannel");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  // let OOCSI check to process incoming data
  oocsi.check();
}
