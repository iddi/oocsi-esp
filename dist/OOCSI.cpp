/***************************************************************************
 * The OOCSI library for the ESP8266 is created to conntect the ESP8266
 * to the OOCSI platform (https://github.com/iddi/oocsi). 
 * It allows to send and receive from the OOCSI platform and allows for easy
 * set-up of the ESP8266 platform as an OOCSI client.
 * Built by Jort Band
 **************************************************************************/



#include "OOCSI.h"

OOCSI::OOCSI(String  Name, char* hostServer, void(*func)()){
  //function for setting up the OOSI send 
  //connect to the hostServer and setup the receiver.
  OOCSIName= "";
  OOCSIName.concat(Name);
  OOCSIName.concat(";");
  OOCSIName.concat('\n');
  host = hostServer;
  manageWifi = false;

}

OOCSI::OOCSI(const char* Name, const char* hostServer, const char* Wifissid, const char* Wifipassword, void (*func)()){
  //function for setting up the whole Oocsi enviroment and login on the WiFi network
  ssid = Wifissid;
  password = Wifipassword;
  receivedMessage = false;
  theMessage = "";
  processMessage =func;
  manageWifi = true;

  OOCSIName= "";
  OOCSIName.concat(Name);
  OOCSIName.concat("(JSON)");
  OOCSIName.concat('\n');
  host = hostServer;
  
  
  
}

void OOCSI::connectWifi(){
  //function for connecting wifi, setup and reconnection is handled automagically
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  int counter = 0;
  Serial.print("connecting");
  
  while(WiFi.status() !=WL_CONNECTED && counter<500){
    delay(200);
    counter++;
    Serial.print(".");
  }
  Serial.println();
  if(WiFi.status() !=WL_CONNECTED){
    Serial.println("Wifi connection attempt failed");
  }else{
    Serial.println("Wifi connection established");
  }
  
}

void OOCSI::subscribe(String chan){
  //function for subscribing to an OOCSI channel, function can be called multiple times
  channel = "";
  channel.concat("subscribe ");
  channel.concat( chan);
  client.println(channel);
  //Serial.println(channel);
}

void OOCSI::unsubcribe(String chan){
  channel = "";
  channel.concat("unsubscribe ");
  channel.concat(chan);
  client.println(channel);
}
  
boolean OOCSI::connectOocsi(){
  if(manageWifi){
    connectWifi();
  }
  
  connectionAttemptCounter = 0;
  if(!client.connect(host,port)){
    //then it failed. so do it again
    if(connectionAttemptCounter<100){
      connectOocsi();
      connectionAttemptCounter++;
    }else{
      Serial.println("setting up OOCSI Connection failed");
      return false;
    }
  }else{
    //continue with setting up.
    client.print(OOCSIName);
    //wait for a response from the server.
    while(!client.available()){
      delay(500);
    }
    Serial.println("connected to OOCSI");
    String message;
    message = client.readStringUntil('\r');
    Serial.println(message);
  }
    checkTime = millis();
}

void OOCSI::check(){
  //function which checks for new messages or pings periodically
  String message;
  
  while(client.available()){
    message = client.readStringUntil('\n');
  }
  /*
  Serial.print("Message: ");
  Serial.print(message);
  Serial.print("theindex of ping: ");
  Serial.println(message.indexOf("ping"));
  */
  if(message.indexOf("ping")>=0 || message == " " ){
    //do not process the message
    Serial.println("ping;");
    //client.print("sendraw hello hello=hello\n");
  }else if(message.length()>0){
    theMessage = message;
    receivedMessage = true;
    processMessage();
  }
    
  //send '.' at least every 60's, to maintain connection with OOCSI server
  //checks and sends a dot at least every 30s
    if(millis() - checkTime> 30000){
        checkTime = millis();
        client.println(".");
    }
  
}

void OOCSI::printMessage(){
  //function which prints the entire incoming message
  Serial.print(theMessage);
}

int OOCSI::getInt(String key, int standard){
  
  int index = theMessage.indexOf(key);
  if(index ==-1){
    return standard;
  }else{
    index+= key.length();
    index = index +2; //to get past the ": chars
    int endindex = theMessage.indexOf(' ', index) -1; //search for the first seperation char (a space)
    //Serial.print("theindex is: ");
    //Serial.println(index);
    String numbers = theMessage.substring(index, endindex+1);
    int result = numbers.toInt();
    return result;
  }
}

float OOCSI::getFloat(String key, float standard){
  int index = theMessage.indexOf(key);
  //Serial.print("theindex is: ");
  //Serial.println(index);
  if(index ==-1){
    return standard;
  }else{
    index+= key.length();
    index = index +2; //to get past the ": chars
    int endindex = theMessage.indexOf(' ', index) -1; //search for the first seperation char (a space)
    //Serial.print("theindex is: ");
    //Serial.println(index);
    String numbers = theMessage.substring(index, endindex+1);
    float result = numbers.toFloat();
    return result;
  }
}

String OOCSI::getString(String key, String standard){
  int index = theMessage.indexOf(key);
  if(index ==-1){
    return standard;
  }else{
    index+= key.length() + 3; //to get past the ": chars
    int endindex = theMessage.indexOf('"', index); //search for the end char a: "
    String result = theMessage.substring(index, endindex);
    return result;
  }
}

void  OOCSI::getIntArray(String key, int standard[], int* passArray, int arrayLength){
  //array opens with [, closes with ], seperation with: , so example:"data:[1,2,3,4]"
  int index = theMessage.indexOf(key);
  if(index ==-1){
    passArray = standard;
  }else{
    index += key.length() + 3;
    int closingindex = theMessage.indexOf("]", index);
    int endindex = theMessage.indexOf(",", index);
    boolean breakAfter = false;
    for(int i = 0; i< arrayLength; i++){
      passArray[i] = theMessage.substring(index, endindex).toInt();
      index = endindex + 1;
      endindex = theMessage.indexOf(",", index);
      if(breakAfter){
        break;
      }
      if(endindex == -1 || endindex>=closingindex){
        breakAfter = true;
      }
    }
  }
}

void OOCSI::getFloatArray(String key, float standard[], float* passArray, int arrayLength){
   //array opens with [, closes with ], seperation with: , so example:"data:[1,2,3,4]"
  int index = theMessage.indexOf(key);
  if(index ==-1){
    passArray = standard;
  }else{
    index += key.length() + 3;
    int closingindex = theMessage.indexOf("]", index);
    int endindex = theMessage.indexOf(",", index);
    boolean breakAfter = false;
    for(int i = 0; i< arrayLength; i++){
      passArray[i] = theMessage.substring(index, endindex).toFloat();
      index = endindex + 1;
      endindex = theMessage.indexOf(",", index);
      if(breakAfter){
        break;
      }
      if(endindex == -1 || endindex>=closingindex){
        breakAfter = true;
      }
    }
  }
}

void OOCSI::getStringArray(String key, String standard[], String* passArray, int arrayLength){
  int index = theMessage.indexOf(key);
  if(index ==-1){
    passArray = standard;
  }else{
    index += key.length() + 3;
    int closingindex = theMessage.indexOf("]" , index);
    int endindex = theMessage.indexOf(",", index);
    boolean breakAfter = false;
    for(int i = 0; i<arrayLength; i++){
      passArray[i] = theMessage.substring(index, endindex);
      index = endindex + 1;
      endindex = theMessage.indexOf(",",index);
      if(breakAfter){
        break;
      }
      if(endindex == -1 || endindex >=closingindex){
        breakAfter = true;
      }
    }
  }
}

void OOCSI::newMessage(String receiver){
  //function for setting up a new message for sending over OOCSI
  //basically clears the old message and sets the receiver
  messageReceiver = receiver;
  outgoingMessage = "sendraw ";
  outgoingMessage.concat(receiver);
  outgoingMessage.concat(" {");
  firstval=true;
}

void OOCSI::sendInt(String key, int value){
  //function for sending an int to OOCSI
  if(firstval){
    outgoingMessage.concat('"');
    firstval = false;
  }else{
    outgoingMessage.concat(",");
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(":");
  outgoingMessage.concat('"');
  String no = String(value,DEC);
  outgoingMessage.concat(no);
  outgoingMessage.concat('"');
}

void OOCSI::sendFloat(String key, float value){
  //function for sending a float to OOCSI
  if(firstval){
    outgoingMessage.concat('"');
    firstval = false;
  }else{
    outgoingMessage.concat(",");
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(":");
  outgoingMessage.concat('"');
  String no = String(value,DEC);
  outgoingMessage.concat(no);
  outgoingMessage.concat('"');
}
void OOCSI::sendString(String key, String value){
  if(firstval){
    outgoingMessage.concat('"');
    firstval = false;
  }else{
    outgoingMessage.concat(",");
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(":");
  outgoingMessage.concat('"');
  outgoingMessage.concat(value);
  outgoingMessage.concat('"');
}
void OOCSI::sendIntArray(String key, int* value, int len){
  //function for sending an array of ints
  if(firstval){
    outgoingMessage.concat('"');
    firstval = false;
  }else{
    outgoingMessage.concat(",");
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(":");
  outgoingMessage.concat('[');
  for(int i = 0; i<len; i++){
    String no = String(value[i],DEC);
    outgoingMessage.concat(no);
    if(i<len-1){
      outgoingMessage.concat(',');
    }
  }
  outgoingMessage.concat(']');
}
void OOCSI::sendFloatArray(String key, float* value, int len){
   //function for sending an array of floats
  if(firstval){
    outgoingMessage.concat('"');
    firstval = false;
  }else{
    outgoingMessage.concat(",");
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(":");
  outgoingMessage.concat('[');
  for(int i = 0; i<len; i++){
    String no = String(value[i],DEC);
    outgoingMessage.concat(no);
    if(i<len-1){
      outgoingMessage.concat(',');
    }
  }
  outgoingMessage.concat(']');
}

void OOCSI::sendStringArray(String key, String* value, int len){
  //function for sending an array of Strings
  if(firstval){
    outgoingMessage.concat('"');
    firstval = false;
  }else{
    outgoingMessage.concat(",");
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(":");
  outgoingMessage.concat('[');
  for(int i = 0; i<len; i++){
    outgoingMessage.concat('"');
    outgoingMessage.concat(value[i]);
    outgoingMessage.concat('"');
    if(i<len-1){
      outgoingMessage.concat(',');
    }
  }
  outgoingMessage.concat(']');
}

void OOCSI::sendMessage(){
  outgoingMessage.concat('}');
  client.println(outgoingMessage);
}

void OOCSI::printSendMessage(){
  Serial.println(outgoingMessage);
}

