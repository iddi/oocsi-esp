/***************************************************************************
 * The OOCSI library for the ESP8266 is created to conntect the ESP8266
 * to the OOCSI platform (https://github.com/iddi/oocsi). 
 * It allows to send and receive from the OOCSI platform and allows for easy
 * set-up of the ESP8266 platform as an OOCSI client.
 * Built by Jort Band
 **************************************************************************/
 
 #ifndef OOCSI_h
#define OOCSI_h

#include <ESP8266WiFi.h>
class OOCSI{
  public:
    //setup functions
    OOCSI(String  Name, char* hostServer, void (*func)());
    OOCSI(const char* Name, const char* hostServer, const char* Wifissid, const char* wifipassword, void (*func)());
    boolean connectOocsi();
    void check();
    void subscribe(String chan);
    void unsubcribe(String chan);

    //retrieve data functions
    void printMessage();
    int getInt(String key, int standard);
    float getFloat(String key, float standard);
    String getString(String key, String standard);
    void getIntArray(String key, int standard[], int* passArray, int arrayLength);
    void getFloatArray(String key, float standard[], float* passArray, int arrayLength);
    void getStringArray(String key, String standard[], String* passArray, int arrayLength);

    //send data functions
    void newMessage(String receiver);
    void sendInt(String key, int value);
    void sendFloat(String key, float value);
    void sendString(String key, String value);
    void sendIntArray(String key, int* value, int len);
    void sendFloatArray(String key, float* value, int len);
    void sendStringArray(String key, String* value, int len);
    void sendMessage();
    void printSendMessage();


  private:
    WiFiClient client;
    boolean manageWifi;
    void connectWifi();
    void checkPing();
    const uint16_t port = 4444; 
    String theMessage;
    String outgoingMessage;
    String messageReceiver;
    boolean firstval;
    String OOCSIName;
    const char*  host;
    const char* ssid;
    const char* password;
    String channel;
    uint16_t connectionAttemptCounter;
    boolean receivedMessage;
    void (*processMessage)();
    boolean OOCSIConnected;
    unsigned long checkTime;
};

#endif
