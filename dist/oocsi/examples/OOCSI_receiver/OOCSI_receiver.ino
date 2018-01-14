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
const char* OOCSIName = "ESP_OOCSI_CLIENT_RECEIVER";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();

// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);

  // output OOCSI activity on pin 5 (LED)
  pinMode(5, OUTPUT);
  oocsi.setActivityLEDPin(5);

  // use this to switch off logging to Serial
  // oocsi.setLogging(false);

  // setting up OOCSI. processOOCSI is the name of the fucntion to call when receiving messages, can be a random function name
  // connect wifi and OOCSI to the server
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);

  // subscribe to a channel
  Serial.println("subscribing to testchannel");
  oocsi.subscribe("testchannel");

  // check if we are in the client list
  Serial.print("is ");
  Serial.print(OOCSIName);
  Serial.print(" a client? --> ");
  Serial.println(oocsi.containsClient(OOCSIName));
}

void loop() {
  // put your main code here, to run repeatedly:

  // let OOCSI check to process incoming data
  oocsi.check();
}

// function which OOCSI calls when an OOCSI message is received
void processOOCSI() {

  // printing the output of different variables of the message; standard call is get<data type>(key, standard value)
  // the standard value -200 will be returned when the key is not included in the OOCSI message
  Serial.print("string: " );
  Serial.print(oocsi.getString("string", "-200"));
  Serial.print("\tfloat: ");
  Serial.print(oocsi.getFloat("float", -200.0));
  Serial.print("\t int: ");
  Serial.print(oocsi.getInt("integer", -200));
  Serial.print("\t long: ");
  Serial.print(oocsi.getInt("long", -200));
  Serial.print("\t intArray: ");

  // printing out an array requires you to pass the maximum length of the array 
  // and an array to be used to display the results in
  int standardarray[] = {1, 2};
  int results[] = {0, 0};
  oocsi.getIntArray("array", standardarray, results, 3);
  Serial.print(results[0]);
  Serial.print(',');
  Serial.print(results[1]);

  float standAr[] = {1.0, 2.0};
  float res[] = {0.0, 0.0};
  oocsi.getFloatArray("array", standAr, res, 2);
  Serial.print("\t floatArray: ");
  Serial.print(res[0]);
  Serial.print(", ");
  Serial.print(res[1]);

  Serial.print("\t sender: ");
  Serial.print(oocsi.getSender());
  Serial.print("\t recipient: ");
  Serial.print(oocsi.getRecipient());
  Serial.print("\t Timestamp: ");
  Serial.print(oocsi.getTimeStamp());
  Serial.println();
}
