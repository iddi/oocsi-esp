/****************************************************************************
   Example of the OOCSI-ESP library connecting to WiFi and receiving time 
   information over OOCSI. This works out of hte box with an OOCSI server and
   you will receive time information either as a String or split up into 
   different fields: (y)ear, (M)onth, (d)ay, (h)our, (m)inute, and (s)econd.
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
  oocsi.connect(OOCSIName, hostserver, ssid, password, processTimeInfo);

  // subscribe to a channel
  Serial.println("Subscribing to timechannel");
  oocsi.subscribe("timechannel");
}

void loop() {
  // put your main code here, to run repeatedly:
  // ...

  // let OOCSI check to process incoming data
  oocsi.check();
}

// function which OOCSI calls when an OOCSI message is received
void processTimeInfo() {

  // printing the output the time information, first as a date-time string:
  Serial.print("date and time as string: ");
  Serial.print(oocsi.getString("datetime", ""));
  
  // then the different components, one-by-one:
  Serial.print("\t year: ");
  Serial.print(oocsi.getInt("y", -1));
  Serial.print("\t month: ");
  Serial.print(oocsi.getInt("M", -1));
  Serial.print("\t day: ");
  Serial.print(oocsi.getInt("d", -1));
  Serial.print("\t hour: ");
  Serial.print(oocsi.getInt("h", -1));
  Serial.print("\t minute: ");
  Serial.print(oocsi.getInt("m", -1));
  Serial.print("\t second: ");
  Serial.print(oocsi.getInt("s", -1));

  // use this to print out the raw message that was received
  // oocsi.printMessage();
}
