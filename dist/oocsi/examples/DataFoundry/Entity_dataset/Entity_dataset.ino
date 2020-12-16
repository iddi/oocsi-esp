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
DFDataset entity(datafoundry, 2, "hmktGR2B9fWbOLYIzB2jrFCmXn7HVJzN/1eutNdGtnSJ1XZe");

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

  // you can also already set the item and token here,
  // if they don't change throughout the program execution
  //  entity.id("mydevice");
  //  entity.token("mydevice");
}

void loop() {

  // identify the item in the entity dataset
  // id is used to identify the item
  entity.id("mydevice");
  // token cannot be empty, can act as a password if needed
  // use same string as id if password protection is not needed
  entity.token("mydevice");

  // fill in some data for the item (very similar to OOCSI)
  entity.addInt("numericalTimingData", 0);

  // add the item (initially or after delete)
  entity.addItem();

  delay(10000);

  // retrieve the item
  entity.getItem();

  // extract the data (very similar to OOCSI)
  int data = entity.getInt("numericalTimingData", -1);
  Serial.println(data);

  delay(10000);

  // add or update data
  entity.addInt("numericalTimingData", millis());
  entity.addString("stringData", "a long, long, long, long, long string - not really, but you get the point.");

  // update the item with new data
  entity.updateItem();

  delay(10000);

  // remove the item
  entity.deleteItem();

  delay(10000);
}
