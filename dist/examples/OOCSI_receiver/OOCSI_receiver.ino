/**************************************************************************
 * Example of the OOCSI library connecting to WiFi and receiving messages
 * over OOCSI. Designed to work with the processing OOCSI DataSender example
 **************************************************************************/
            
#include <ESP8266WiFi.h>
#include "OOCSI.h"

const char* OOCSIName = "ESP826_OOCSI_CLIENT";    //name for connecting with OOCSI
const char* hostserver = "your.OOCSI.Server";     //put the adress of your OOCSI server here, can be a web page or an IP adres string
const char* ssid = "yourSSID";                    //SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* password = "yourPassword";            //Password of your Wifi network.
const char* OOCSIChannel = "datachannel";          //name of the channel you want to subscribe to (can subscribe to multiple channels)

OOCSI oocsi(OOCSIName, hostserver, ssid, password, proccessOOCSI);  //setting up OOCSI. processOOCSI is the name of the fucntion to call when receiving messages, can be a random function name.


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("connecting Wifi");
  oocsi.connectOocsi();           //connect wifi and OOCSI to the server.
  Serial.println("subscribing");
  oocsi.subscribe(OOCSIChannel);        //subscribe to a channel
  
}

void loop() {
  // put your main code here, to run repeatedly:
  oocsi.check();  //function needs to be checked in order for OOCSI to process incoming data.
}

void proccessOOCSI(){
  //function which OOCSI calls when an OOCSI message is received.
  oocsi.printMessage(); //function for printing the raw incoming message (JSON string).
  Serial.println();

  //printing the output of different variables of the message.
  //Standard setup is (key, standardvalue). The standard value will be returned when the key is not included in the OOCSI message.
  Serial.print("string: " );
  Serial.print(oocsi.getString("string","-200")); 
  Serial.print("\tfloat: ");
  Serial.print(oocsi.getFloat("float",-200.0));
  Serial.print("\t int: ");
  Serial.print(oocsi.getInt("integer",-200));
  Serial.print("\t intArray: ");

  //printing out an Array requires you to pass the maximum length of the array and an array to be used to display the results in.
  int standardarray[] = {1,2};
  int results[] = {0,0};
  oocsi.getIntArray("array", standardarray, results, 3);
  Serial.print(results[0]);
  Serial.print(',');
  Serial.print(results[1]);

  
  float standAr[] = {1.0,2.0};
  float res[] = {0.0,0.0};
  oocsi.getFloatArray("array", standAr, res, 2);
  Serial.print("\t floatArray: ");
  Serial.print(res[0]);
  Serial.print(", ");
  Serial.print(res[1]);
  Serial.println();
}
