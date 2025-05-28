/***************************************************************************
 * The DataFoundry library for the ESP32, ESP8266, Arduino MKR Wifi 101, 
 * Arduino UNO Wifi, Arduino Nano 33 IoT, and Arduino Nano RP2040 
 * to store and retrieve data from ESPs and Arduinos on the Data 
 * Foundry platform.
 *
 * Developed by Mathias Funk
 * 
 * Last updated @May 27, 2025 by Eden Chiang 
 * New support models: Arduino Nano ESP32, and Arduino Uno R4 WiFi
 * 
 * Known issue: 
 * ESP8266 is only available for logging data to IoT dataset via logItem(), 
 * other access to Entity dataset, such as addItem(), editItem(), 
 * deleteItem(), and getItem() will get error message from Data Foundry
 **************************************************************************/

#ifndef DFDATASET_h
#define DFDATASET_h

#if defined(ESP32) || defined(NANO_ESP32)
    #include <WiFi.h>
    #include <HTTPClient.h>
#endif
#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClientSecureBearSSL.h>
#endif
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_NANO_RP2040_CONNECT)
    #include <WiFiNINA.h>
    #include <ArduinoHttpClient.h>
#endif
#ifdef ARDUINO_UNOWIFIR4
    #include <WiFiS3.h>
    #include <ArduinoHttpClient.h>
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

    // wifi connection
    WiFiClient wifi;

    // configuration
    const char* host;
    char address[200];
    const char* api_token;
    const char* device_id;
    const char* activity_id;
    const char* resource_id;
    const char* resource_token;
    int dataset_id = 0;

    // data
    JsonDocument jsonDocument;
    JsonDocument jsonMessage;
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
    void urlencode(char* dst, const char* src);
};

#endif
