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
  processMessage = func;
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
  boolean connected = false;

  while(WiFi.status() !=WL_CONNECTED && counter<500){

    if(WiFi.status() == WL_CONNECTED){
        if(counter>0){
            Serial.println();
            Serial.println("Wifi connection established");
        }
        connected = true;
        break;
    }
      delay(200);
      counter++;
      Serial.print(".");
  }
    if(!connected && WiFi.status() !=WL_CONNECTED ){
        Serial.println("Wifi connection attempt failed");
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

void OOCSI::unsubscribe(String chan){
  channel = "";
  channel.concat("unsubscribe ");
  channel.concat(chan);
  client.println(channel);
}

boolean OOCSI::connectOocsi(){
  if(manageWifi){
    connectWifi();
  }

   //Serial.println("connecting client: ");

  static int connectionAttemptCounter = 0;
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
    prevTime = millis();
    if(message.indexOf("welcome") ==-1){
      //already connected once before, wait a second then connect again
      delay(2000);
      connectOocsi();
    }

    return true;
  }
}

void OOCSI::check(){
  //function which checks for new messages or pings periodically
  //check if we are connected

  if(WiFi.status() !=WL_CONNECTED){
    //if not connect oocsi again
    connectOocsi();
  }

  if(!client.connected()){
    //reconnect client
    connectOocsi();
  }

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

  if(millis()-prevTime>30000){
    client.println(".");
    prevTime = millis();
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

long OOCSI::getLong(String key, long standard){

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
    long result = numbers.toInt();
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


String OOCSI::getSender(){
  int index = theMessage.indexOf("sender");
  String result;
  if(index ==-1){
      result = "";
    return result;
  }else{
    index += 9; //sender has 6 chars + 3
    int closingindex = theMessage.indexOf('"' , index);
    result = theMessage.substring(index,closingindex);
    return result;
  }
}

String OOCSI::getRecipient(){
  int index = theMessage.indexOf("recipient");
  String result;
  if(index ==-1){
      result = "";
    return result;
  }else{
    index += 9; //sender has 6 chars + 3
    int closingindex = theMessage.indexOf('"' , index);
    result = theMessage.substring(index,closingindex);
    return result;
  }
}

long OOCSI::getTimeStamp(){
  int index = theMessage.indexOf("timestamp");
  if(index ==-1){
    return 0;
  }else{
    index+= 9;  //timestamp is nine chars long
    index = index +2; //to get past the ": chars
    int endindex = theMessage.indexOf(',', index) -1; //search for the first seperation char (a comma)
    //Serial.print("theindex is: ");
    //Serial.println(index);
      //check if numbers is longer then 10 (to prevent overflowing the number)
      Serial.print("startindex: ");
      Serial.print(index);
      Serial.print("\tendindex: ");
      Serial.print(endindex);
      if(endindex-index > 10){
          //cut the string
          index = endindex - 9;
      }
      Serial.print("\tstartindex2: ");
      Serial.println(index);
    String numbers = theMessage.substring(index, endindex+1);
      Serial.print("temp timestamp: ");
      Serial.println(numbers);

    long result = numbers.toInt();
    return result;
  }
}

boolean OOCSI::has(String key){
  int index = theMessage.indexOf(key);
  if(index == -1){
    return false;
  }else{
    return true;
  }
}

String OOCSI::keys(){
  //function for outputting all the keys in the message
  //after{ and before [, then after ]

  int i = 0;
  //first search for the first entry and the { , [ and ] symbols
  int comma = theMessage.indexOf(",");
  int bracket = theMessage.indexOf("{");
  int endofKey = theMessage.indexOf(":");

  //first key
  String thekeys = "";
  //get substring of key
  String firstKey = theMessage.substring(bracket+2, endofKey-2);
  thekeys.concat(firstKey);

  String restMessage = theMessage.substring(endofKey + 1);
  boolean gotKeys = true;

  while(gotKeys){
    //go through the message.
    //check for the smallest sentence
    //finding the next either after a , or {
    comma = restMessage.indexOf(",");
    bracket = restMessage.indexOf("{");
    endofKey = restMessage.indexOf(":");

    //check if there still is a key otherwise break
    if(comma == -1 && bracket == -1){
      gotKeys = false;
      break;
    }
    String tempkey;
    if(bracket>comma){
      //use the comma as entry point
      tempkey = restMessage.substring(comma + 2, endofKey -2); //+2 to skip over

    }else{
      tempkey = restMessage.substring(bracket + 2, endofKey -2);
    }

    thekeys.concat(", ");
    thekeys.concat(tempkey);
    //now reitterate again.
    restMessage = restMessage.substring(endofKey +1);
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
  }else{
    outgoingMessage.concat(",");
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(":");
  //outgoingMessage.concat('"');
  String no = String(value,DEC);
  outgoingMessage.concat(no);
  //outgoingMessage.concat('"');
}
void OOCSI::sendLong(String key, long value){
    //function for sending an int to OOCSI
    if(firstval){
        outgoingMessage.concat('"');
    }else{
        outgoingMessage.concat(",");
        outgoingMessage.concat('"');
    }
    outgoingMessage.concat(key);
    outgoingMessage.concat('"');
    outgoingMessage.concat(":");
    //outgoingMessage.concat('"');
    String no = String(value,DEC);
    outgoingMessage.concat(no);
    //outgoingMessage.concat('"');
}

void OOCSI::sendFloat(String key, float value){
  //function for sending a float to OOCSI
  if(firstval){
    outgoingMessage.concat('"');
  }else{
    outgoingMessage.concat(",");
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(":");
  //outgoingMessage.concat('"');
  String no = String(value,DEC);
  outgoingMessage.concat(no);
  //outgoingMessage.concat('"');
}
void OOCSI::sendString(String key, String value){
  if(firstval){
    outgoingMessage.concat('"');
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

String OOCSI::getClients(){
  //basically send a message and then wait for the response client list.
  //first read the standard messages
  check();
  //now send and receive
  client.println("clients");
  String message;
  //keep waiting for a message
  while(!client.available()){
    delay(20);
  }
  while(client.available()){
    message = client.readStringUntil('\n');
  }
  return message;
}

String OOCSI::getChannels(){
  //basically send a message and then wait for the response channels list.
  //first read the standard messages
  check();
  //now send and receive
  client.println("channels");
  String message;
  //keep waiting for a message
  while(!client.available()){
    delay(20);
  }
  while(client.available()){
    message = client.readStringUntil('\n');
  }
  return message;
}

boolean OOCSI::containsClient(String clientName){
  //check for the client.
  String clientlist = getClients();
  if(clientlist.indexOf(clientName) == -1){
    //not found
    return false;
  }

  return true;
}
/*
void OOCSI::removeSlashes(){
  int slashloc = theMessage.indexOf((char) 92);
  while(slashloc != -1)
  theMessage.remove(slashloc);  //92 is a: \
  slashloc = theMessage.indexOf((char) 92);
}*/
