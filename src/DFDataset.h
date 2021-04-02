/***************************************************************************
 * The DataFoundry library for the ESP32 and ESP8266 to store and retrieve
 * data from ESPs on the Data Foundry platform.
 *
 * Developed by Mathias Funk
 **************************************************************************/

#ifndef DFDATASET_h
#define DFDATASET_h

#ifdef ESP32
    #include <WiFi.h>
    #include <HTTPClient.h>
    #ifndef MSG_SIZE
        #define MSG_SIZE 2048
    #endif
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #ifndef MSG_SIZE
        #define MSG_SIZE 1024
    #endif
#endif
#ifdef ARDUINO_SAMD_NANO_33_IOT
    #include <WiFiNINA.h>
    #include <ArduinoHttpClient.h>
    #ifndef MSG_SIZE
        #define MSG_SIZE 1024
    #endif
#endif

#ifndef LED_BUILTIN
    #define LED_BUILTIN -1
#endif
#include <ArduinoJson.h>

class DFDataset {    

  public:

    // setup
    DFDataset(const char* host, int id, const char* token);

    // iot datasets
    void device(const char* act);
    void activity(const char* act);
    // entity datasets
    void id(const char* act);
    void token(const char* act);

    // sending data
    DFDataset clear();
    DFDataset addBool(const char* key, bool value);
    DFDataset addInt(const char* key, int value);
    DFDataset addLong(const char* key, long value);
    DFDataset addFloat(const char* key, float value);
    DFDataset addString(const char* key, const char* value);
    
    // operations
    bool logItem();
    bool addItem();
    bool updateItem();
    bool deleteItem();
    bool getItem();

    // receiving data
    bool getBool(const char* key, bool standard);
    int getInt(const char* key, int standard);
    long getLong(const char* key, long standard);
    float getFloat(const char* key, float standard);
    String getString(const char* key, const char* standard);
    bool has(const char* key);
    String keys();

    // misc
    void printMessage();
    void setActivityLEDPin(int ledPin);
    void setLogging(bool log);

  private:

    // configuration
    const char* host;
    char address[200];
    const char* api_token;
    const char* device_id;
    const char* activity_id;
    const char* resource_id;
    const char* resource_token;
    int dataset_id = 0;

#ifdef ARDUINO_SAMD_NANO_33_IOT
    WiFiClient wifi;
#endif

    // data
    StaticJsonDocument<MSG_SIZE> jsonDocument;
    StaticJsonDocument<MSG_SIZE> jsonMessage;
    String jsonMessageReceiver;
    bool receivedMessage;

    // logging
    int activityLEDPin;
    bool logging;
    void print(const String &message);
    void print(char message);
    void println();
    void println(const String &message);
    void println(char message);
};

#endif
