/***************************************************************************
 * The OOCSI library for the ESP32 and ESP 8266 is created to connect ESPs
 * to the OOCSI platform (https://github.com/iddi/oocsi).
 * It allows to send and receive from the OOCSI platform and allows for easy
 * set-up of the ESP32 and ESP8266 platforms as OOCSI clients.
 *
 * Developed by Jort Band, Mathias Funk
 **************************************************************************/

#include "OOCSI.h"
#include "OOCSIJsonParser.h"
#include "JsonStreamingParser.h"


// function for creating an OOCSI client
OOCSI::OOCSI() {
  activityLEDPin = -1;
  logging = true;

  parser.setListener(&listener);
}

// function for connecting first to OOCSI
boolean OOCSI::connect(const char* name, const char* hostServer, void(*func)()) {
  receivedMessage = false;
  theMessage = "";
  theMessage.reserve(4096);
  processMessage = func;
  manageWifi = false;

  OOCSIName = "";
  OOCSIName.concat(name);
  OOCSIName.concat("(JSON)");
  OOCSIName.concat('\n');
  host = hostServer;

  internalConnect();
}

// function for connecting first to WIFI then to OOCSI
boolean OOCSI::connect(const char* name, const char* hostServer, const char* Wifissid, const char* Wifipassword, void (*func)()) {
  receivedMessage = false;
  theMessage = "";
  theMessage.reserve(4096);
  processMessage = func;
  ssid = Wifissid;
  password = Wifipassword;
  manageWifi = true;

  OOCSIName = "";
  OOCSIName.concat(name);
  OOCSIName.concat("(JSON)");
  OOCSIName.concat('\n');
  host = hostServer;

  internalConnect();
}

void OOCSI::disconnect() {
    client.println("quit");
    client.stop();
    theMessage = "";
    WiFi.disconnect();
}

// function for connecting wifi, setup and reconnection is handled automagically
void OOCSI::connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int counter = 0;
  println("Connecting to Wifi");
  boolean connected = false;

  while (WiFi.status() != WL_CONNECTED && counter++ < 50) {  yield();
    if (WiFi.status() == WL_CONNECTED) {
      if (counter > 0) {
        println("Wifi connection established");
      }
      connected = true;
      break;
    }

    if(activityLEDPin > -1) {
      digitalWrite(activityLEDPin, HIGH);
      delay(20);
      digitalWrite(activityLEDPin, LOW);
      delay(20);
      digitalWrite(activityLEDPin, HIGH);
      delay(20);
      digitalWrite(activityLEDPin, LOW);
      delay(100);
    }
    print(".");

    // new line
    if (counter % 80 == 0) {
      println("Connecting to Wifi");
    }

    // wait before next attempt
    delay(200);
  }
  if (!connected && WiFi.status() != WL_CONNECTED ) {
    println("Wifi connection attempt failed");
  }
}

// function for subscribing to an OOCSI channel, function can be called multiple times
void OOCSI::subscribe(const char* chan) {
  String channel = "";
  channel.concat("subscribe ");
  channel.concat(chan);
  client.println(channel);
}

// function for unsubscribing from an OOCSI channel, function can be called multiple times
void OOCSI::unsubscribe(const char* chan) {
  String channel = "";
  channel.concat("unsubscribe ");
  channel.concat(chan);
  client.println(channel);
}

// function to internally connect to OOCSI (needs all variables set up)
boolean OOCSI::internalConnect() {
  
  // keep connecting the wifi
  while (manageWifi && WiFi.status() != WL_CONNECTED) {  yield();
    connectWifi();
  }

  // only then connect OOCSI
  println();
  println("Connecting to OOCSI");

  static int connectionAttemptCounter = 0;
  if (!client.connect(host, port)) {
    // it failed. so do it again
    if (connectionAttemptCounter++ < 100) {
      internalConnect();
      print(".");
    } else {
      println("setting up OOCSI Connection failed");
      return false;
    }
  } else {
    // continue with client-server handshake
    client.println(OOCSIName);
    client.setTimeout(500);

    // wait for a response from the server (max. 20 sec)
    int waitingResponseCounter = 0;
    while (!client.available() && waitingResponseCounter++ < 40) {  yield();
      print(".");
      delay(200);
    }

    if (client.available()) {
      String message = client.readStringUntil('\n');
      println(message);
      prevTime = millis();

      if (message.indexOf("welcome") == -1) {
        // wait before connecting again
        delay(1000);
        return internalConnect();
      }

      return true;
    } else {
      return false;
    }
  }
}

// function which checks for new messages or pings periodically
void OOCSI::check() {
  // check if we are connected to the WIFI
  if (WiFi.status() != WL_CONNECTED) {
    // if not connect oocsi again
    internalConnect();
  }
  if (!client.connected()) {
    // reconnect client
    internalConnect();
  }

  String message = "";
  message.reserve(1024);
  if (client.available() && message.length() == 0) {
    message = client.readStringUntil('\n');
  }

  int pingIndex = message.indexOf("ping");
  if ((pingIndex >= 0 && pingIndex < 10) || message == " " ) {
    // it's a heart beat, send one back
    client.println(".\n");
    println("ping");
    prevTime = millis();
  } else if (message.length() > 0) {
    // process the message
    theMessage = message;
    receivedMessage = true;
    processMessage();
    
    if(activityLEDPin > -1) {
      delay(20);
      digitalWrite(activityLEDPin, HIGH);
      delay(20);
      digitalWrite(activityLEDPin, LOW);
      delay(20);
      digitalWrite(activityLEDPin, HIGH);
      delay(20);
      digitalWrite(activityLEDPin, LOW);
    }
  }

  if (millis() - prevTime > 30000) {
    client.println(".");
    prevTime = millis();
  }
}

// function which prints the entire incoming message
void OOCSI::printMessage() {
  print(theMessage);
}

bool OOCSI::getBool(const char* key, bool standard) {
  String result = listener.get(theMessage, key);
  return result.length() > 0 ? result.equalsIgnoreCase("true") : standard;
}

int OOCSI::getInt(const char* key, int standard) {
  String result = listener.get(theMessage, key);
  return result.length() > 0 ? result.toInt() : standard;
}

long OOCSI::getLong(const char* key, long standard) {
  String result = listener.get(theMessage, key);
  return result.length() > 0 ? (long) result.toInt() : standard;
}

float OOCSI::getFloat(const char* key, float standard) {
  String result = listener.get(theMessage, key);
  return result.length() > 0 ? result.toFloat() : standard;
}

String OOCSI::getString(const char* key, const char* standard) {
  String result = listener.get(theMessage, key);
  return result.length() > 0 ? result : standard;
}

void  OOCSI::getBoolArray(const char* key, bool standard[], bool* passArray, int arrayLength) {
  String array = listener.getArray(theMessage, key);

  if (array.length() == 0) {
    passArray = standard;
  } else {
    int index = 0;
    int endindex = array.indexOf(',', index);
    int closingindex = array.length() - 1;    
    for (int i = 0; i < arrayLength; i++) {
      passArray[i] = array.substring(index, endindex).equalsIgnoreCase("true");

      index = endindex + 1;
      endindex = array.indexOf(',', index);
      if (endindex == -1 && i < arrayLength - 1) {
        passArray[i+1] = array.substring(index).equalsIgnoreCase("true");
        break;
      }
    }
  }
}

void  OOCSI::getIntArray(const char* key, int standard[], int* passArray, int arrayLength) {
  String array = listener.getArray(theMessage, key);

  if (array.length() == 0) {
    passArray = standard;
  } else {
    int index = 0;
    int endindex = array.indexOf(',', index);
    int closingindex = array.length() - 1;    
    for (int i = 0; i < arrayLength; i++) {
      passArray[i] = array.substring(index, endindex).toInt();

      index = endindex + 1;
      endindex = array.indexOf(',', index);
      if (endindex == -1 && i < arrayLength - 1) {
        passArray[i+1] = array.substring(index).toInt();        
        break;
      }
    }
  }
}

void OOCSI::getFloatArray(const char* key, float standard[], float* passArray, int arrayLength) {
  String array = listener.getArray(theMessage, key);

  if (array.length() == 0) {
    passArray = standard;
  } else {
    int index = 0;
    int endindex = array.indexOf(',', index);
    int closingindex = array.length() - 1;    
    for (int i = 0; i < arrayLength; i++) {
      passArray[i] = array.substring(index, endindex).toFloat();

      index = endindex + 1;
      endindex = array.indexOf(',', index);
      if (endindex == -1 && i < arrayLength - 1) {
        passArray[i+1] = array.substring(index).toFloat();        
        break;
      }
    }
  }
}

void OOCSI::getStringArray(const char* key, const char* standard[], char* passArray[], int arrayLength) {
  String array = listener.getArray(theMessage, key);

  if (array.length() == 0) {
    for (int i = 0; i < arrayLength; i++) {
      String temp = String(standard[i]);
      temp.toCharArray(passArray[i], temp.length());
    }
  } else {
    int index = 0;
    int endindex = array.indexOf(',', index);
    int closingindex = array.length() - 1;    
    for (int i = 0; i < arrayLength; i++) {
      array.substring(index + 1, endindex - 1).toCharArray(passArray[i], endindex-index);

      index = endindex + 1;
      endindex = array.indexOf(',', index);
      if (endindex == -1 && i < arrayLength - 1 && index < closingindex) {
        Serial.println(array.substring(index + 1, closingindex));
        array.substring(index + 1, closingindex).toCharArray(passArray[i+1], closingindex - (index + 1));
        break;
      }
    }
  }
}

String OOCSI::getSender() {
  return listener.get(theMessage, "sender");
}

String OOCSI::getRecipient() {
  return listener.get(theMessage, "recipient");
}

long OOCSI::getTimeStamp() {
  String numbers = listener.get(theMessage, "timestamp");
  return numbers.length() > 9 ? numbers.substring(0, 10).toInt() : numbers.toInt();
}

boolean OOCSI::has(const char* key) {
  return listener.has(theMessage, key);
}

String OOCSI::keys() {
  return listener.keys(theMessage);
}

// function for setting up a new message for sending over OOCSI
OOCSI OOCSI::newMessage(const char* receiver) {
  //basically clears the old message and sets the receiver
  outgoingMessage = "sendraw ";
  outgoingMessage.concat(receiver);
  outgoingMessage.concat(" {");
  firstval = true;

  return *this;
}

// function for sending a bool to OOCSI
OOCSI OOCSI::addBool(const char* key, bool value) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  String no = value ? "true" : "false";
  outgoingMessage.concat(no);

  return *this;
}

// function for sending an int to OOCSI
OOCSI OOCSI::addInt(const char* key, int value) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  String no = String(value, DEC);
  outgoingMessage.concat(no);

  return *this;
}

// function for sending an int to OOCSI
OOCSI OOCSI::addLong(const char* key, long value) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  String no = String(value, DEC);
  outgoingMessage.concat(no);

  return *this;
}

// function for sending a float to OOCSI
OOCSI OOCSI::addFloat(const char* key, float value) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  String no = String(value, DEC);
  outgoingMessage.concat(no);

  return *this;
}

OOCSI OOCSI::addString(const char* key, const char* value) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  outgoingMessage.concat('"');
  outgoingMessage.concat(value);
  outgoingMessage.concat('"');

  return *this;
}

// function for sending an array of bools
OOCSI OOCSI::addBoolArray(const char* key, bool* value, int len) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  outgoingMessage.concat('[');
  for (int i = 0; i < len; i++) {
    String no = value[i] ? "true" : "false";
    outgoingMessage.concat(no);
    if (i < len - 1) {
      outgoingMessage.concat(',');
    }
  }
  outgoingMessage.concat(']');

  return *this;
}

// function for sending an array of ints
OOCSI OOCSI::addIntArray(const char* key, int* value, int len) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  outgoingMessage.concat('[');
  for (int i = 0; i < len; i++) {
    String no = String(value[i], DEC);
    outgoingMessage.concat(no);
    if (i < len - 1) {
      outgoingMessage.concat(',');
    }
  }
  outgoingMessage.concat(']');

  return *this;
}

// function for sending an array of floats
OOCSI OOCSI::addFloatArray(const char* key, float* value, int len) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  outgoingMessage.concat('[');
  for (int i = 0; i < len; i++) {
    String no = String(value[i], DEC);
    outgoingMessage.concat(no);
    if (i < len - 1) {
      outgoingMessage.concat(',');
    }
  }
  outgoingMessage.concat(']');

  return *this;
}

//function for sending an array of Strings
OOCSI OOCSI::addStringArray(const char* key, const char* value, int len) {
  if (firstval) {
    outgoingMessage.concat('"');
    firstval = false;
  } else {
    outgoingMessage.concat(',');
    outgoingMessage.concat('"');
  }
  outgoingMessage.concat(key);
  outgoingMessage.concat('"');
  outgoingMessage.concat(':');
  outgoingMessage.concat('[');
  for (int i = 0; i < len; i++) {
    outgoingMessage.concat('"');
    outgoingMessage.concat(value[i]);
    outgoingMessage.concat('"');
    if (i < len - 1) {
      outgoingMessage.concat(',');
    }
  }
  outgoingMessage.concat(']');

  return *this;
}

// close and send out the message
void OOCSI::sendMessage() {
  outgoingMessage.concat('}');
  outgoingMessage.concat('\n');
  client.println(outgoingMessage);
}

// log the outgoing message
void OOCSI::printSendMessage() {
  println(outgoingMessage);
}


String OOCSI::getClients() {
  //basically send a message and then wait for the response client list.
  //first read the standard messages
  check();
  //now send and receive
  client.println("clients");
  String message;
  //keep waiting for a message
  while (!client.available()) {  yield();
    delay(20);
  }
  while (client.available()) {  yield();
    message = client.readStringUntil('\n');
  }
  return message;
}

String OOCSI::getChannels() {
  //basically send a message and then wait for the response channels list.
  //first read the standard messages
  check();
  //now send and receive
  client.println("channels");
  String message;
  //keep waiting for a message
  while (!client.available()) {  yield();
    delay(20);
  }
  while (client.available()) {  yield();
    message = client.readStringUntil('\n');
  }
  return message;
}

boolean OOCSI::containsClient(const char* clientName) {
  //check for the client.
  String clientlist = getClients();
  if (clientlist.indexOf(clientName) == -1) {
    //not found
    return false;
  }

  return true;
}

void OOCSI::print(const String &message) {
  if(logging) {
    Serial.print(message);
  }
}

void OOCSI::print(char message) {
  if(logging) {
    Serial.print(message);
  }
}

void OOCSI::println() {
  if(logging) {
    Serial.println();
  }
}

void OOCSI::println(const String &message) {
  if(logging) {
    Serial.print("\nOOCSI: ");
    Serial.print(message);
  }
}

void OOCSI::println(char message) {
  if(logging) {
    Serial.print("\nOOCSI: ");
    Serial.print(message);
  }
}

void OOCSI::setActivityLEDPin(int ledPin) {
  activityLEDPin = ledPin;
}

void OOCSI::setLogging(boolean log) {
  logging = log;
}

/*
  void OOCSI::removeSlashes(){
  int slashloc = theMessage.indexOf((char) 92);
  while(slashloc != -1)
  theMessage.remove(slashloc);  //92 is a: \
  slashloc = theMessage.indexOf((char) 92);
  }
*/
