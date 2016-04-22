/**************************************************************************
 * Example of the OOCSI library connecting to WiFi and sending messages
 * over OOCSI. 
 **************************************************************************/
            
#include <ESP8266WiFi.h>
#include "OOCSI.h"

const char* OOCSIName = "ESP826_OOCSI_CLIENT";    //name for connecting with OOCSI
const char* hostserver = "your.OOCSI.Server";     //put the adress of your OOCSI server here, can be a web page or an IP adres string
const char* ssid = "yourSSID";                    //SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* password = "yourPassword";            //Password of your Wifi network.
const char* OOCSIChannel = "datachannel";       //name of the channel you want to subscribe to (can subscribe to multiple channels)

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
  oocsi.newMessage((String) OOCSIChannel);  //
  oocsi.sendInt("int", 40);
  oocsi.sendFloat("float", 50.0);
  oocsi.sendString("string" , "hello world");
  int theIntArray[] = {45,55,60};
  oocsi.sendIntArray("intArray" ,theIntArray , 3);
  float theFloatArray[] = {55.0,65.0,75.0};
  oocsi.sendFloatArray("floatArray", theFloatArray, 3);
  String stringArrayToSend[] = {"Batman", "Superman", "Deadpool"};
  oocsi.sendStringArray("stringArray", stringArrayToSend, 3);
  oocsi.sendMessage();
  oocsi.printSendMessage();   //prints out the message how it is send to the OOCSI server
  oocsi.check();  //function needs to be checked in order for OOCSI to process incoming data.
  delay(500);
}

void proccessOOCSI(){
}
