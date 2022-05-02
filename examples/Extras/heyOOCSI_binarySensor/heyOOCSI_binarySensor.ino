/******************************************************************************
   Example of the OOCSI-ESP library connecting to OOCSI over WiFi and creating 
   an OOCSI device with a location and a binary sensor component that is then
   registered. Loop() shows how to set the state the binary sensor component.
 ******************************************************************************/

#include "OOCSI.h"
#include "OOCSIDevice.h"

// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_sayHi_test_binary_sensor";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();
OOCSIDevice binarySensor = oocsi.heyOOCSI();

// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);

  // output OOCSI activity on the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  oocsi.setActivityLEDPin(LED_BUILTIN);

  // use this to switch off logging to Serial
  // oocsi.setLogging(false);

  // setting up OOCSI. processOOCSI is the name of the function to call when receiving messages, can be a random function name
  // connect wifi and OOCSI to the server
  oocsi.connect(OOCSIName, hostserver, ssid, password);

  // configure the binary sensor with a textual location label and numerical coordinates
  binarySensor.addLocation("hallway", 380, 430);
  // add a binary sensor, for example, a sensor that senses if a door is open or not
  // parameters: name, channel for updates, type, default is off (false), and a "door" icon
  binarySensor.addBinarySensor("front_door_open", "hallway", "door", false, "door");
  // register binary sensor on server
  binarySensor.sayHi();
}

// put your main code here, to run repeatedly:
void loop() {

  // switch the binary sensor state randomly
  binarySensor.setState("front_door_open", random(0, 2) < 1);

  // let OOCSI keep the connection to the OOCSI server fresh and crunchy
  // use this keepAlive() function if you do NOT need to receive data from OOCSI
  // use the check() function if you also need to process incoming messages
  oocsi.keepAlive();

  // wait 2.5 secs for the next run
  delay(2500);
}
