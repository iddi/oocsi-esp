/******************************************************************************
   Example of the OOCSI-ESP library connecting to OOCSI over WiFi and creating 
   an OOCSI device with a location and a light component that is then
   registered. Loop() shows how to set the state and brightness of the light.
 ******************************************************************************/

#include "OOCSI.h"
#include "OOCSIDevice.h"

// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_sayHi_test_light";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();
OOCSIDevice light = oocsi.heyOOCSI();

int lightBrightness = 150;
bool lightState = false;

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
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);

  // configure the light with a textual location label and numerical coordinates
  light.addLocation("living_room", 340, 240);
  // add a light
  // parameters: name, channel for updates, LED type (RGBW), spectrum type (0), default off, default brightness 150 and a "lightbulb" icon
  light.addLight("ceiling_light", "living_room", "RGBW", 0, false, 150, "lightbulb");
  // register light on server
  light.sayHi();
}

// put your main code here, to run repeatedly:
void loop() {

  // we can let the light blink, or control from another OOCSI client over the "living_room" channel
  light.setState("ceiling_light", millis() % 2000 > 1000);
  // we can also control the brightness directly, or via another OOCSI client
  light.setValue("ceiling_light", "brightness", 150 + sin(millis()) * 50);

  // let OOCSI keep the connection to the OOCSI server fresh and crunchy
  // use this keepAlive() function if you do NOT need to receive data from OOCSI
  // use the check() function if you also need to process incoming messages
  oocsi.keepAlive();

  // wait 2.5 secs for the next run
  delay(2500);
}

// use this callback function to receive data for the light
void processOOCSI() {
  // check if the data is for the light component
  if(oocsi.has("component") && oocsi.getString("component", "") == "ceiling_light") {
    // retrieve the data, first check brightness
    if(oocsi.has("brightness")) {
      lightBrightness = oocsi.getInt("brightness", 150);
    }
    // retrieve the data, first check brightness
    if(oocsi.has("value")) {
      lightState = oocsi.getBool("value", false);
    }
  }
}
