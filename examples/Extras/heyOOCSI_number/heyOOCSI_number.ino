/******************************************************************************
   Example of the OOCSI-ESP library connecting to OOCSI over WiFi and creating
   an OOCSI device with a location and a number component that is then
   registered. Loop() shows how to set the value the number component.
 ******************************************************************************/

#include "OOCSI.h"
#include "OOCSIDevice.h"

// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_sayHi_test_number";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();
OOCSIDevice ac = oocsi.heyOOCSI();

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

  // configure the number component with a textual location label and numerical coordinates
  ac.addLocation("home", 100, 420);
  // add a number component that, for instance, sets temperature on an AC unit
  // parameters: name, channel for updates, default is 0, unit is degrees Celsius ("C") and it has an "air-conditioner" icon
  ac.addNumber("ac_temperature", "home", 0, "C", "air-conditioner");
  // register number component on server
  ac.sayHi();
}

// put your main code here, to run repeatedly:
void loop() {

  // set the number value randomly
  ac.setValue("ac_temperature", random(18, 25));

  // let OOCSI keep the connection to the OOCSI server fresh and crunchy
  // use this keepAlive() function if you do NOT need to receive data from OOCSI
  // use the check() function if you also need to process incoming messages
  oocsi.keepAlive();

  // wait 2.5 secs for the next run
  delay(2500);

//  // if you need to update the location, that's no problem
//  ac.addLocation("home", 200, 320);
//  ac.sayHi();
}
