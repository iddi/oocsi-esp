/***************************************************************************
 * The OOCSI library for the ESP32, ESP8266, Arduino MKR Wifi 101, 
 * Arduino UNO Wifi, Arduino Nano 33 IoT, and Arduino Nano RP2040 to 
 * connect to the OOCSI platform (https://github.com/iddi/oocsi).
 * It allows to send and receive from the OOCSI platform and allows for easy
 * setup of ESPs and Arduinos as OOCSI clients.
 *
 * Developed by Jort Band, Mathias Funk
 **************************************************************************/

#ifndef OOCSI_h
#define OOCSI_h

#ifdef ESP32
    #include <WiFi.h>
    #ifndef MSG_SIZE
        #define MSG_SIZE 2048
    #endif
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #ifndef MSG_SIZE
        #define MSG_SIZE 1024
    #endif
#endif
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_NANO_RP2040_CONNECT)
    #include <WiFiNINA.h>
    #ifndef MSG_SIZE
        #define MSG_SIZE 1024
    #endif
#endif


#ifndef LED_BUILTIN
    #define LED_BUILTIN -1
#endif
#include <ArduinoJson.h>

class OOCSIDevice;
class OOCSI {
  typedef void(*oocsiCallbackFunction_t)(void*);

  public:

    // setup, connection, subscription
    OOCSI();
    bool connect(const char* Name, const char* hostServer);
    bool connect(const char* Name, const char* hostServer, void (*func)());
    bool connect(const char* Name, const char* hostServer, const char* Wifissid, const char* wifipassword);
    bool connect(const char* Name, const char* hostServer, const char* Wifissid, const char* wifipassword, void (*func)());
    bool connect(const char* Name, const char* hostServer, oocsiCallbackFunction_t callback, void* callbackData);
    bool connect(const char* Name, const char* hostServer, const char* Wifissid, const char* wifipassword, oocsiCallbackFunction_t callback, void* callbackData);
    void disconnect();
    bool check();
    void keepAlive();
    void subscribe(const char* chan);
    void unsubscribe(const char* chan);
    OOCSIDevice heyOOCSI();
    OOCSIDevice heyOOCSI(const char* name);

    // sending data
    OOCSI newMessage(const char* receiver);
    OOCSI addBool(const char* key, bool value);
    OOCSI addInt(const char* key, int value);
    OOCSI addLong(const char* key, long value);
    OOCSI addFloat(const char* key, float value);
    OOCSI addString(const char* key, const char* value);
    OOCSI addBoolArray(const char* key, bool* value, int len);
    OOCSI addIntArray(const char* key, int* value, int len);
    OOCSI addLongArray(const char* key, long* value, int len);
    OOCSI addFloatArray(const char* key, float* value, int len);
    OOCSI addStringArray(const char* key, String value[], int len);
    JsonObject addJsonObject(const char* key);
    void setJsonObject(const char* key, JsonObject obj);
    JsonArray addJsonArray(const char* key);
    void setJsonArray(const char* key, JsonArray arr);
    void sendMessage();
    void printSendMessage();

    // receiving data
    String getSender();
    String getRecipient();
    bool getBool(const char* key, bool standard);
    int getInt(const char* key, int standard);
    long getLong(const char* key, long standard);
    float getFloat(const char* key, float standard);
    String getString(const char* key, const char* standard);
    void getBoolArray(const char* key, bool* standard, bool* passArray, int arrayLength);
    void getIntArray(const char* key, int* standard, int* passArray, int arrayLength);
    void getLongArray(const char* key, long* standard, long* passArray, int arrayLength);
    void getFloatArray(const char* key, float* standard, float* passArray, int arrayLength);
    void getStringArray(const char* key, String standard[], String passArray[], int arrayLength);
    long getTimeStamp();
    bool has(const char* key);
    String keys();
    void printMessage();

    // misc functions
    void setActivityLEDPin(int ledPin);
    void setLogging(bool log);
    String getClients();
    String getChannels();
    bool containsClient(const char* clientName);
    String getName();


  private:

    // WIFI
    const char* ssid;
    const char* password;
    bool manageWifi = false;

    // OOCSI
    const char* OOCSIName;
    const char* host;
    const uint16_t port = 4444;
    WiFiClient client;
    long prevTime;

    // messaging
    StaticJsonDocument<MSG_SIZE> jsonDocument;
    StaticJsonDocument<MSG_SIZE> jsonMessage;
    String jsonMessageReceiver;
    bool receivedMessage;
    int activityLEDPin;
    void (*processMessage)();
    oocsiCallbackFunction_t processMessageDataHandle;
    void* processMessageData;

    // logging
    bool logging;
    void print(const String &message);
    void print(char message);
    void println();
    void println(const String &message);
    void println(char message);
    void connectWifi();
    bool internalConnect();
};

#endif
