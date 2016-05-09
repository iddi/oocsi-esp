/***************************************************************************
 * The OOCSI library for the ESP8266 is created to conntect the ESP8266
 * to the OOCSI platform (https://github.com/iddi/oocsi).
 * It allows to send and receive from the OOCSI platform and allows for easy
 * set-up of the ESP8266 platform as an OOCSI client.
 * Built by Jort Band
 **************************************************************************/

//TODO: check for connection periodically if it is lost, reconnect
//TODO: Create a WIFI manager
//* (DONE, TODO: Test)Phase 1: reconnect OOCSI automatically on lost connection
//* Phase 2: allow input of multiple adresses
//TODO: Create String, Int, Float, Long, structs for returning an array with length

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
    void unsubscribe(String chan);                                                           //TODO: test

    //retrieve data functions
    void printMessage();
    int getInt(String key, int standard);
    long getLong(String key, long standard);                                                //TODO: test
    float getFloat(String key, float standard);
    String getString(String key, String standard);
    void getIntArray(String key, int standard[], int* passArray, int arrayLength);          //make it return a string pointer, use struct??, need size.
    void getFloatArray(String key, float standard[], float* passArray, int arrayLength);    //make it return a float pointer
    void getStringArray(String key, String standard[], String* passArray, int arrayLength); //make it return a string pointer
    String getSender();                                                                     //TODO: test
    String getRecipient();                                                                  //TODO: test
    long getTimeStamp();                                                                    //TODO: test                                                                 //TODO: test
    boolean has(String key);                                                                //TODO: test
    String keys();                                                                          //TODO: test



    //send data functions
    void newMessage(String receiver);
    void sendInt(String key, int value);
    void sendLong(String key, long value);
    void sendFloat(String key, float value);
    void sendString(String key, String value);
    void sendIntArray(String key, int* value, int len);
    void sendFloatArray(String key, float* value, int len);
    void sendStringArray(String key, String* value, int len);
    void sendMessage();
    void printSendMessage();

    //misc functions
    String getClients();                                                                  //TODO: test
    String getChannels();                                                                 //TODO: test/create
    boolean containsClient(String clientName);                                          //TODO: test
    //void removeSlashes(); //To cope with random popping up slashes                        //TODO: test



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
    //uint16_t connectionAttemptCounter ;
    boolean receivedMessage;
    void (*processMessage)();
    boolean OOCSIConnected;
    long prevTime;
};

#endif
