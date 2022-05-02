/*********************************************************************
   Example of the ESP connecting to WiFi and accessing items in an
   Entity dataset on the Data Foundry.
   This works only with an existing account, project and dataset on
   the Data Foundry (https://data.id.tue.nl)
 *********************************************************************/

#include "DFDataset.h"

// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// put the adress of Data Foundry here
const char* datafoundry = "data.id.tue.nl";

// create connection to dataset with server address, dataset id, and the access token
DFDataset iot(datafoundry, 2, "Gg+txzAGalvfox1BQqwlHpDFIsM2nm5U9enMOF2mA/W382zyuM3JC");

// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);

  // establish Wifi connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print('.');
  }

  Serial.println("Connected to the WiFi network");

  // you can also already set the device and activity here,
  // if they don't change throughout the program execution
  //  iot.device("mydevice");
  //  iot.activity("idle");
}

void loop() {

  // specify device, can be empty
  iot.device("mydevice");

  // specify activity, can be empty
  iot.activity("idle");

  // fill in some data for the item (very similar to OOCSI)
  iot.addInt("numericalTimingData", millis());
  iot.addBool("yesno", true);
  iot.addString("stringData", "a long, long, long, long, long string - not really, but you get the point.");

  // log the item data to the dataset
  iot.logItem();

  delay(10000);
}