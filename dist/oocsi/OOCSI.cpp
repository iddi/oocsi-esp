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

// function for connecting wifi, setup and reconnection is handled automagically
void OOCSI::connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int counter = 0;
  print("Connecting to Wifi");
  boolean connected = false;

  while (WiFi.status() != WL_CONNECTED && counter++ < 500) {
    if (WiFi.status() == WL_CONNECTED) {
      if (counter > 0) {
        println();
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
      println();
      print("Connecting to Wifi");
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
  if (manageWifi) {
    connectWifi();
  }

  println();
  println("Connecting to OOCSI");

  static int connectionAttemptCounter = 0;
  if (!client.connect(host, port)) {
    //then it failed. so do it again
    if (connectionAttemptCounter++ < 100) {
      internalConnect();
    } else {
      println("setting up OOCSI Connection failed");
      return false;
    }
  } else {
    // continue with client-server handshake
    client.print(OOCSIName);

    // wait for a response from the server (max. 20 sec)
    int waitingResponseCounter = 0;
    while (!client.available() && waitingResponseCounter++ < 40) {
      delay(500);
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

      return true;
    } else {
      return false;
    }
  }
}

// function which checks for new messages or pings periodically
void OOCSI::check() {
  //check if we are connected to the WIFI
  if (WiFi.status() != WL_CONNECTED) {
    //if not connect oocsi again
    internalConnect();
  }
  if (!client.connected()) {
    //reconnect client
    internalConnect();
  }

  String message;
  while (client.available() && message.length() == 0) {
    message = client.readStringUntil('\n');
  }

  if (message.indexOf("ping") >= 0 || message == " " ) {
    // it's a heart beat, send one back
    println("ping;");
    client.println(".\n");
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
  int index = theMessage.indexOf(key);
  if (index == -1) {
    return standard;
  } else {
    index += strlen(key);
    // to get past the ": chars
    index = index + 2;
    // search for the first separation char
    int endindex = getNextStopChar(index);
    String numbers = theMessage.substring(index, endindex);
    bool result = numbers.equalsIgnoreCase("true") ? true : false;
    return result;
  }
}

int OOCSI::getInt(const char* key, int standard) {
  int index = theMessage.indexOf(key);
  if (index == -1) {
    return standard;
  } else {
    index += strlen(key);
    // to get past the ": chars
    index = index + 2;
    // search for the first separation char
    int endindex = getNextStopChar(index);
    String numbers = theMessage.substring(index, endindex);
    int result = numbers.toInt();
    return result;
  }
}

long OOCSI::getLong(const char* key, long standard) {
  int index = theMessage.indexOf(key);
  if (index == -1) {
    return standard;
  } else {
    index += strlen(key);
    // to get past the ": chars
    index = index + 2;
    // search for the first separation char
    int endindex = getNextStopChar(index);
    String numbers = theMessage.substring(index, endindex);
    long result = numbers.toInt();
    return result;
  }
}

float OOCSI::getFloat(const char* key, float standard) {
  int index = theMessage.indexOf(key);
  //print("theindex is: ");
  //println(index);
  if (index == -1) {
    return standard;
  } else {
    index += strlen(key);
    // to get past the ": chars
    index = index + 2;
    // search for the first separation char
    int endindex = getNextStopChar(index);
    String numbers = theMessage.substring(index, endindex);
    float result = numbers.toFloat();
    return result;
  }
}

String OOCSI::getString(const char* key, const char* standard) {
  int index = theMessage.indexOf(key);
  if (index == -1) {
    return standard;
  } else {
    // to get past the ":" chars
    index += strlen(key) + 3;
    // search for the end char a: "
    int endindex = theMessage.indexOf('"', index);
    String result = theMessage.substring(index, endindex);
    return result;
  }
}

void  OOCSI::getBoolArray(const char* key, bool standard[], bool* passArray, int arrayLength) {
  //array opens with [, closes with ], seperation with: , so example:"data:[true,false,true,false]"
  int index = theMessage.indexOf(key);
  if (index == -1) {
    passArray = standard;
  } else {
    index += strlen(key) + 3;
    int closingindex = theMessage.indexOf("]", index);
    int endindex = theMessage.indexOf(',', index);
    boolean breakAfter = false;
    for (int i = 0; i < arrayLength; i++) {
      passArray[i] = theMessage.substring(index, endindex).equalsIgnoreCase("true");
      index = endindex + 1;
      endindex = theMessage.indexOf(',', index);
      if (breakAfter) {
        break;
      }
      if (endindex == -1 || endindex >= closingindex) {
        breakAfter = true;
      }
    }
  }
}

void  OOCSI::getIntArray(const char* key, int standard[], int* passArray, int arrayLength) {
  //array opens with [, closes with ], seperation with: , so example:"data:[1,2,3,4]"
  int index = theMessage.indexOf(key);
  if (index == -1) {
    passArray = standard;
  } else {
    index += strlen(key) + 3;
    int closingindex = theMessage.indexOf("]", index);
    int endindex = theMessage.indexOf(',', index);
    boolean breakAfter = false;
    for (int i = 0; i < arrayLength; i++) {
      passArray[i] = theMessage.substring(index, endindex).toInt();
      index = endindex + 1;
      endindex = theMessage.indexOf(',', index);
      if (breakAfter) {
        break;
      }
      if (endindex == -1 || endindex >= closingindex) {
        breakAfter = true;
      }
    }
  }
}

void OOCSI::getFloatArray(const char* key, float standard[], float* passArray, int arrayLength) {
  //array opens with [, closes with ], seperation with: , so example:"data:[1,2,3,4]"
  int index = theMessage.indexOf(key);
  if (index == -1) {
    passArray = standard;
  } else {
    index += strlen(key) + 3;
    int closingindex = theMessage.indexOf("]", index);
    int endindex = theMessage.indexOf(',', index);
    boolean breakAfter = false;
    for (int i = 0; i < arrayLength; i++) {
      passArray[i] = theMessage.substring(index, endindex).toFloat();
      index = endindex + 1;
      endindex = theMessage.indexOf(',', index);
      if (breakAfter) {
        break;
      }
      if (endindex == -1 || endindex >= closingindex) {
        breakAfter = true;
      }
    }
  }
}

// void OOCSI::getStringArray(const char* key, char* standard[], char* passArray[], int arrayLength) {
//   int index = theMessage.indexOf(key);
//   if (index == -1) {
//     passArray = standard;
//   } else {
//     index += strlen(key) + 3;
//     int closingindex = theMessage.indexOf("]" , index);
//     int endindex = theMessage.indexOf(',', index);
//     boolean breakAfter = false;
//     for (int i = 0; i < arrayLength; i++) {
//       passArray[i] = theMessage.substring(index, endindex).c_str();
//       index = endindex + 1;
//       endindex = theMessage.indexOf(',', index);
//       if (breakAfter) {
//         break;
//       }
//       if (endindex == -1 || endindex >= closingindex) {
//         breakAfter = true;
//       }
//     }
//   }
// }

String OOCSI::getSender() {
  return getString("sender", "");
}

String OOCSI::getRecipient() {
  return getString("recipient", "");
}

long OOCSI::getTimeStamp() {
  return getLong("timestamp", -1);
}

boolean OOCSI::has(const char* key) {
  int index = theMessage.indexOf(key);
  if (index == -1) {
    return false;
  } else {
    return true;
  }
}

String OOCSI::keys() {
  //function for outputting all the keys in the message
  //after{ and before [, then after ]

  int i = 0;
  //first search for the first entry and the { , [ and ] symbols
  int comma = theMessage.indexOf(',');
  int bracket = theMessage.indexOf('{');
  int endofKey = theMessage.indexOf(':');

  //first key
  String thekeys = "";
  //get substring of key
  String firstKey = theMessage.substring(bracket + 2, endofKey - 2);
  thekeys.concat(firstKey);

  String restMessage = theMessage.substring(endofKey + 1);
  boolean gotKeys = true;

  while (gotKeys) {
    //go through the message.
    //check for the smallest sentence
    //finding the next either after a , or {
    comma = restMessage.indexOf(',');
    bracket = restMessage.indexOf('{');
    endofKey = restMessage.indexOf(':');

    //check if there still is a key otherwise break
    if (comma == -1 && bracket == -1) {
      gotKeys = false;
      break;
    }
    String tempkey;
    if (bracket > comma) {
      //use the comma as entry point
      tempkey = restMessage.substring(comma + 2, endofKey - 2); //+2 to skip over

    } else {
      tempkey = restMessage.substring(bracket + 2, endofKey - 2);
    }

    thekeys.concat(", ");
    thekeys.concat(tempkey);
    //now reitterate again.
    restMessage = restMessage.substring(endofKey + 1);
  }
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
  while (!client.available()) {
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

boolean OOCSI::containsClient(const char* clientName) {
  //check for the client.
  String clientlist = getClients();
  if (clientlist.indexOf(clientName) == -1) {
    //not found
    return false;
  }

  return true;
}

int OOCSI::getNextStopChar(int startIndex) {
    int ws = theMessage.indexOf(' ', startIndex);
    int cm = theMessage.indexOf(',', startIndex);
    int cb = theMessage.indexOf('}', startIndex);

    int endindex = theMessage.length() - 1;

    if(ws > -1) {
      endindex = ws < endindex ? ws : endindex;
    }
    if(cm > -1) {
      endindex = cm < endindex ? cm : endindex;
    }
    if(cb > -1) {
      endindex = cb < endindex ? cb : endindex;
    }

    return endindex;

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
