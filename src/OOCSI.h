/***************************************************************************
 * The OOCSI library for the ESP32 and ESP 8266 is created to connect ESPs
 * to the OOCSI platform (https://github.com/iddi/oocsi).
 * It allows to send and receive from the OOCSI platform and allows for easy
 * set-up of the ESP32 and ESP8266 platforms as OOCSI clients.
 *
 * Developed by Jort Band, Mathias Funk
 **************************************************************************/

#ifndef OOCSI_h
#define OOCSI_h

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

class OOCSI{
  public:

    // setup, connection, subscription
    OOCSI();
    boolean connect(const char* Name, const char* hostServer, void (*func)());
    boolean connect(const char* Name, const char* hostServer, const char* Wifissid, const char* wifipassword, void (*func)());
    void check();
    void subscribe(const char* chan);
    void unsubscribe(const char* chan);                                                           //TODO: test

    // sending data
    OOCSI newMessage(const char* receiver);
    OOCSI addInt(const char* key, int value);
    OOCSI addLong(const char* key, long value);
    OOCSI addFloat(const char* key, float value);
    OOCSI addString(const char* key, const char* value);
    OOCSI addIntArray(const char* key, int* value, int len);
    OOCSI addFloatArray(const char* key, float* value, int len);
    OOCSI addStringArray(const char* key, const char* value, int len);
    void sendMessage();
    void printSendMessage();

    // receiving data
    String getSender();                                                                     //TODO: test
    String getRecipient();                                                                  //TODO: test
    int getInt(const char* key, int standard);
    long getLong(const char* key, long standard);                                                //TODO: test
    float getFloat(const char* key, float standard);
    String getString(const char* key, const char* standard);
    void getIntArray(const char* key, int standard[], int* passArray, int arrayLength);          //make it return a string pointer, use struct??, need size.
    void getFloatArray(const char* key, float standard[], float* passArray, int arrayLength);    //make it return a float pointer
    // void getStringArray(const char* key, char* standard[], char* passArray[], int arrayLength); //make it return a string pointer
    long getTimeStamp();                                                                    //TODO: test                                                                 //TODO: test
    boolean has(const char* key);                                                                //TODO: test
    String keys();                                                                          //TODO: test
    void printMessage();
    void setActivityLEDPin(int ledPin);
    void setLogging(boolean log);

    // misc functions
    String getClients();                                                                  //TODO: test
    String getChannels();                                                                 //TODO: test/create
    boolean containsClient(const char* clientName);                                          //TODO: test
    //void removeSlashes(); //To cope with random popping up slashes                        //TODO: test


  private:
    // OOCSI
    String OOCSIName;
    const char* host;
    const uint16_t port = 4444;
    boolean OOCSIConnected;
    long prevTime;

    // WIFI
    const char* ssid;
    const char* password;
    WiFiClient client;
    boolean manageWifi;

    // messaging
    String theMessage;
    String outgoingMessage;
    boolean receivedMessage;
    boolean firstval;
    int activityLEDPin;
    boolean logging;

    void print(const String &message);
    void print(char message);
    void println();
    void println(const String &message);
    void println(char message);
    void connectWifi();
    boolean internalConnect();
    void (*processMessage)();
};

#endif
