/******************************************************************************
   Example of the OOCSI-ESP library connecting to OOCSI over WiFi and creating 
   an OOCSI device with a location and a switch component that is then
   registered. Loop() shows how to set the state of the switch component.
 ******************************************************************************/

#include "OOCSI.h"
#include "OOCSIDevice.h"

// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_sayHi_test_switch";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();
OOCSIDevice socket = oocsi.heyOOCSI();

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

  // configure the socket with a textual location label and numerical coordinates
  socket.addLocation("living_room", 280, 290);
  // add a switch, like a socket or light switch
  // parameters: name, channel for updates, default value, icon
  socket.addSwitch("socket_switch", "living_room", false, "light-switch");
  // register socket switch on server
  socket.sayHi();
}

// put your main code here, to run repeatedly:
void loop() {

  // we toggle the socket every 5 seconds
  socket.setState("socket_switch", millis() % 10000 > 5000);

  // let OOCSI keep the connection to the OOCSI server fresh and crunchy
  // use this keepAlive() function if you do NOT need to receive data from OOCSI
  // use the check() function if you also need to process incoming messages
  oocsi.keepAlive();
  delay(500);
}
