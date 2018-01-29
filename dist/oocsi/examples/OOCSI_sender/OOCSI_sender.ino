/******************************************************************************
   Example of the OOCSI-ESP library connecting to WiFi and sending messages
   over OOCSI. Designed to work with the Processing OOCSI receiver example
   that is provided in the same directory
 ******************************************************************************/

#include "OOCSI.h"

// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_OOCSI_CLIENT_SENDER";
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

  // setting up OOCSI. processOOCSI is the name of the function to call when receiving messages, can be a random function name
  // connect wifi and OOCSI to the server
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
}

// put your main code here, to run repeatedly:
void loop() {
  // create a new message
  oocsi.newMessage("esp-testchannel");

  // add data (primitive data types int, float, long, string)
  // the labels such as "count" or "timestamp" are completely free to choose
  oocsi.addString("greeting" , "hello world!");
  oocsi.addInt("count", 40);
  oocsi.addFloat("float_point", sin(millis()));
  oocsi.addLong("time", (long) millis());

  // sending arrays
  int theIntArray[] = {45, 55, 60};
  oocsi.addIntArray("intArray" , theIntArray , 3);

  float theFloatArray[] = {55.0, 65.0, 75.0};
  oocsi.addFloatArray("floatArray", theFloatArray, 3);

  // this command will send the message; don't forget to call this after creating a message
  oocsi.sendMessage();

  // prints out the raw message (how it is sent to the OOCSI server)
  // oocsi.printSendMessage();

  // needs to be checked in order for OOCSI to process incoming data.
  oocsi.check();
  delay(500);
}

void processOOCSI() {
  // don't do anything; we are sending only
}
