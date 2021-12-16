/***************************************************************************
 * The OOCSIDevice library for the ESP32, ESP8266, Arduino MKR Wifi 101, 
 * Arduino UNO Wifi, Arduino Nano 33 IoT, and Arduino Nano RP2040 to 
 * connect to the OOCSI platform (https://github.com/iddi/oocsi).
 * It allows to register ESP and Arduino-based OOCSI clients as annotated 
 * devices on the OOCSI platform (and HomeAssistant).
 *
 * Developed by Diede van Marle, Mathias Funk
 **************************************************************************/

#ifndef OOCSIDevice_h
#define OOCSIDevice_h
#include "OOCSI.h"
#include <ArduinoJson.h>

#define LIGHT_SPECTRUM_RGB 1
#define LIGHT_SPECTRUM_CCT 2
#define LIGHT_SPECTRUM_WHITE 3
#define LIGHT_SPECTRUM_RGB_CCT 4
#define LIGHT_SPECTRUM_CCT_WHITE 5
#define LIGHT_SPECTRUM_RGB_WHITE 6
#define LIGHT_SPECTRUM_RGB_CCT_WHITE 7

class OOCSI;
class OOCSIDevice {
  public:
  	// setup, connection, subscription
    OOCSIDevice(const char* prototypeName, OOCSI &client, const char* OOCSIName);

    void addProperty(const char* property, const char* propertyValue);
    void addLocation(const char* locationName, const char* latitude,const char* longitude);
    void addSensor(const char* sensorName , const char* sensorChannel, const char* sensorType, const char* sensorDefaultValue, const char* sensorUnit, const char* icon);
    void addNumber(const char* numberName , const char* numberChannel, const char* numberMin, const char* numberMax, const char* numberDefaultValue, const char* numberUnit, const char* icon);
    void addBinarySensor(const char* sensorName , const char* sensorChannel, const char* sensorType, const char* sensorDefaultState, const char* icon);
    void addSwitch(const char* switchName , const char* switchChannel, const char* switchDefaultState, const char* icon);
    void addLight(const char* lightName , const char* lightChannel, const char* ledType, int spectrum, const char* lightDefaultState,  int defaultBrightness, int miredMax, int miredMin, const char* icon);
    void addLight(const char* lightName , const char* lightChannel, const char* ledType, int spectrum, const char* lightDefaultState,  int defaultBrightness, const char* icon);

    void sayHi();

  private:
    OOCSI* oocsi;
    JsonObject device;
    JsonObject properties;
    JsonObject components;
    JsonObject location;
};

#endif