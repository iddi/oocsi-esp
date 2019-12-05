/***************************************************************************
 * The OOCSI library for the ESP32 and ESP 8266 is created to connect ESPs
 * to the OOCSI platform (https://github.com/iddi/oocsi).
 * It allows to send and receive from the OOCSI platform and allows for easy
 * set-up of the ESP32 and ESP8266 platforms as OOCSI clients.
 *
 * Developed by Jort Band, Mathias Funk
 **************************************************************************/

#include "OOCSI.h"

// function for creating an OOCSI client
OOCSI::OOCSI() {
  activityLEDPin = -1;
  logging = true;
}

// function for connecting first to OOCSI
bool OOCSI::connect(const char* name, const char* hostServer, void(*func)()) {
  receivedMessage = false;
  processMessage = func;

  OOCSIName = name;
  host = hostServer;

  internalConnect();
}

// function for connecting first to WIFI then to OOCSI
bool OOCSI::connect(const char* name, const char* hostServer, const char* Wifissid, const char* Wifipassword, void (*func)()) {
  ssid = Wifissid;
  password = Wifipassword;
  manageWifi = true;

  return connect(name, hostServer, func);
}

// function for connecting wifi, setup and reconnection is handled automagically
void OOCSI::connectWifi() {

  if(WiFi.status() == WL_CONNECTED) {
    return;
  }

  print(F("Connecting to Wifi"));

  WiFi.begin(ssid, password);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter++ < 500) {
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
    if (counter % 20 == 0) {
      println();
      print(F("Connecting to Wifi"));
      WiFi.begin(ssid, password);
    }

    // wait before next attempt
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED) {
    println(F("connection attempt failed"));
  }
}

void OOCSI::disconnect() {
    client.println(F("quit"));
    client.stop();
    if (manageWifi) {
      WiFi.disconnect();
    }
}

// function for subscribing to an OOCSI channel, function can be called multiple times
void OOCSI::subscribe(const char* chan) {
  String channel = "subscribe ";
  channel.concat(chan);
  client.println(channel);
}

// function for unsubscribing from an OOCSI channel, function can be called multiple times
void OOCSI::unsubscribe(const char* chan) {
  String channel = "unsubscribe ";
  channel.concat(chan);
  client.println(channel);
}

// function to internally connect to OOCSI (needs all variables set up)
bool OOCSI::internalConnect() {
  if (manageWifi) {
    connectWifi();
  }

  static int connectionAttemptCounter = 0;
  if(connectionAttemptCounter == 0) {
    println();
    print(F("Connecting to OOCSI"));
  }

  if (!client.connect(host, port)) {
    print('_');

    //then it failed. so do it again
    if (connectionAttemptCounter++ < 100) {
      delay(250);
      internalConnect();
    } else {
      println();
      println(F("OOCSI Connection failed"));
      return false;
    }
  } else {
    print("[x] ");
    // continue with client-server handshake
    client.print(OOCSIName);
    client.println(F("(JSON)"));

    // wait for a response from the server (max. 20 sec)
    int waitingResponseCounter = 0;
    while (!client.available() && waitingResponseCounter++ < 40) {
      print('_');
      delay(250);
    }

    if (client.available()) {
      String message = client.readStringUntil('\n');
      println(message);
      prevTime = millis();

      if (message.indexOf("welcome") == -1) {
        // wait before connecting again
        delay(2000);
        return internalConnect();
      }

      println();
      return true;
    } else {
      return false;
    }
  }
}

// function which checks for new messages or pings periodically
void OOCSI::check() {
  //check if we are connected to the WIFI and the OOCSI server
  if (WiFi.status() != WL_CONNECTED || !client.connected()) {
    //if not connect oocsi again
    internalConnect();
  }

  int messageCount = 0;
  String message;
  while (client.available() && messageCount++ < 20) {
    message = client.readStringUntil('\n');

    if (message.indexOf("ping") >= 0 || message == " " ) {
      // it's a heart beat, send one back
      println(F("ping;"));
      client.println('.');
      prevTime = millis();
    } else if (message.length() > 0) {
      // it's a real message, handle it
      DeserializationError error = deserializeJson(jsonDocument, message.c_str());
      if (error) {
        print(F("Message parsing failed: "));
        println(error.c_str());
        receivedMessage = false;
      }
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
    yield();
  }

  // client-side ping
  if (millis() - prevTime > 30000) {
    client.println('.');
    prevTime = millis();
  }
}

// function which prints the entire incoming message
void OOCSI::printMessage() {
  serializeJson(jsonDocument, Serial);
}

bool OOCSI::getBool(const char* key, bool standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<bool>();
  }
}

int OOCSI::getInt(const char* key, int standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<int>();
  }
}

long OOCSI::getLong(const char* key, long standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<long>();
  }
}

float OOCSI::getFloat(const char* key, float standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<float>();
  }
}

String OOCSI::getString(const char* key, const char* standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<String>();
  }
}

void  OOCSI::getBoolArray(const char* key, bool* standard, bool* passArray, int arrayLength) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    passArray = standard;
  } else {
    JsonArray array = jsonDocument[key].as<JsonArray>();
    for(int i = 0; i < arrayLength; i++) {
        passArray[i] = array[i].as<bool>();
    }
  }
}

void  OOCSI::getIntArray(const char* key, int* standard, int* passArray, int arrayLength) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    passArray = standard;
  } else {
    JsonArray array = jsonDocument[key].as<JsonArray>();
    for(int i = 0; i < arrayLength; i++) {
        passArray[i] = array[i].as<int>();
    }
  }
}

void  OOCSI::getLongArray(const char* key, long* standard, long* passArray, int arrayLength) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    passArray = standard;
  } else {
    JsonArray array = jsonDocument[key].as<JsonArray>();
    for(int i = 0; i < arrayLength; i++) {
        passArray[i] = array[i].as<long>();
    }
  }
}

void OOCSI::getFloatArray(const char* key, float* standard, float* passArray, int arrayLength) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    passArray = standard;
  } else {
    JsonArray array = jsonDocument[key].as<JsonArray>();
    for(int i = 0; i < arrayLength; i++) {
        passArray[i] = array[i].as<float>();
    }
  }
}

void OOCSI::getStringArray(const char* key, String standard[], String passArray[], int arrayLength) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    passArray = standard;
  } else {
    JsonArray array = jsonDocument[key].as<JsonArray>();
    for(int i = 0; i < arrayLength; i++) {
        passArray[i] = array[i].as<String>();
    }
  }
}

String OOCSI::getSender() {
  return getString("sender", "");
}

String OOCSI::getRecipient() {
  return getString("recipient", "");
}

long OOCSI::getTimeStamp() {
  return getLong("timestamp", -1);
}

bool OOCSI::has(const char* key) {
  return jsonDocument.containsKey(key);
}

// function for outputting all (top-level) keys in the message as a comma-separated list
String OOCSI::keys() {
  if(jsonDocument.isNull() || jsonDocument.size() == 0) {
    return "";
  }

  // comma separated key list
  String result = "";
  for (JsonPair p : jsonDocument.as<JsonObject>()) {
      if(result.length() > 0) {
        result.concat(",");  
      }
      result.concat(p.key().c_str());
  }
  return result;
}

// function for setting up a new message for sending over OOCSI
OOCSI OOCSI::newMessage(const char* receiver) {
  jsonMessage.clear();
  jsonMessageReceiver = receiver;
  return *this;
}

// function for sending a bool to OOCSI
OOCSI OOCSI::addBool(const char* key, bool value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending an int to OOCSI
OOCSI OOCSI::addInt(const char* key, int value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending an int to OOCSI
OOCSI OOCSI::addLong(const char* key, long value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending a float to OOCSI
OOCSI OOCSI::addFloat(const char* key, float value) {
  jsonMessage[key] = value;
  return *this;
}

OOCSI OOCSI::addString(const char* key, const char* value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending an array of bool values
OOCSI OOCSI::addBoolArray(const char* key, bool* value, int len) {
  JsonArray array = jsonMessage.createNestedArray(key);
  for(int i = 0; i < len; i++) {
    array.add(value[i]);
  }
  return *this;
}

// function for sending an array of int values
OOCSI OOCSI::addIntArray(const char* key, int* value, int len) {
  JsonArray array = jsonMessage.createNestedArray(key);
  for(int i = 0; i < len; i++) {
    array.add(value[i]);
  }
  return *this;
}

// function for sending an array of long values
OOCSI OOCSI::addLongArray(const char* key, long* value, int len) {
  JsonArray array = jsonMessage.createNestedArray(key);
  for(int i = 0; i < len; i++) {
    array.add(value[i]);
  }
  return *this;
}

// function for sending an array of float values
OOCSI OOCSI::addFloatArray(const char* key, float* value, int len) {
  JsonArray array = jsonMessage.createNestedArray(key);
  for(int i = 0; i < len; i++) {
    array.add(value[i]);
  }
  return *this;
}

// function for sending an array of Strings
OOCSI OOCSI::addStringArray(const char* key, String value[], int len) {
  JsonArray array = jsonMessage.createNestedArray(key);
  for(int i = 0; i < len; i++) {
    array.add(value[i]);
  }
  return *this;
}

// close and send out the message
void OOCSI::sendMessage() {
  client.print("sendraw ");
  client.print(jsonMessageReceiver);
  client.print(" ");
  serializeJson(jsonMessage, client);
  client.println();
}

// log the outgoing message
void OOCSI::printSendMessage() {
  if(logging)
    serializeJson(jsonMessage, Serial);
}


String OOCSI::getClients() {
  //basically send a message and then wait for the response client list.
  //first read the standard messages
  check();
  //now send and receive
  client.println("clients");
  String message;
  //keep waiting for a message
  while (!client.available()) {
    yield();
    delay(20);
  }
  while (client.available()) {
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
  while (!client.available()) {
    delay(20);
  }
  while (client.available()) {
    message = client.readStringUntil('\n');
  }
  return message;
}

bool OOCSI::containsClient(const char* clientName) {
  //check for the client.
  String clientlist = getClients();
  if (clientlist.indexOf(clientName) == -1) {
    //not found
    return false;
  }

  return true;
}

void OOCSI::print(const String &message) {
  if(logging)
    Serial.print(message);
}

void OOCSI::print(char message) {
  if(logging)
    Serial.print(message);
}

void OOCSI::println() {
  if(logging)
    Serial.println();
}

void OOCSI::println(const String &message) {
  if(logging)
    Serial.println(message);
}

void OOCSI::println(char message) {
  if(logging)
    Serial.println(message);
}

void OOCSI::setActivityLEDPin(int ledPin) {
  activityLEDPin = ledPin;
}

void OOCSI::setLogging(bool log) {
  logging = log;
}
