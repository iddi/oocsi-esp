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
  	// setup
    OOCSIDevice(const char* prototypeName, OOCSI &client);

    // configure the device
    void addProperty(const char* property, const char* propertyValue);
    void addLocation(const char* locationName, int latitude, int longitude);
    void addSensor(const char* sensorName , const char* sensorChannel, const char* sensorType, int sensorDefaultValue, const char* sensorUnit, const char* icon);
    void addNumber(const char* numberName , const char* numberChannel, float numberDefaultValue, const char* numberUnit, const char* icon);
    void addNumber(const char* numberName , const char* numberChannel, float numberMin, float numberMax, float numberDefaultValue, const char* numberUnit, const char* icon);
    void addBinarySensor(const char* sensorName , const char* sensorChannel, const char* sensorType, bool sensorDefaultState, const char* icon);
    void addSwitch(const char* switchName , const char* switchChannel, bool switchDefaultState, const char* icon);
    void addLight(const char* lightName , const char* lightChannel, const char* ledType, int spectrum, bool lightDefaultState,  int defaultBrightness, int miredMax, int miredMin, const char* icon);
    void addLight(const char* lightName , const char* lightChannel, const char* ledType, int spectrum, bool lightDefaultState,  int defaultBrightness, const char* icon);

    // finalize setup with a single message to OOCSI server
    void sayHi();

    // get value or state of device components after setup
    float getValue(const char* componentName, float defaultValue);
    float getValue(const char* componentName, const char* key, float defaultValue);
    bool getState(const char* componentName, bool defaultState);
    bool getState(const char* componentName, const char* key, bool defaultState);

    // set value or state of device components after setup
    void setValue(const char* componentName, float value);
    void setValue(const char* componentName, const char* key, float value);
    void setState(const char* componentName, bool state);
    void setState(const char* componentName, const char* key, bool state);

  private:
    OOCSI* oocsi;
    const char* prototypeName;

    // just for the sayHi message
    JsonObject device;
    JsonObject properties;
    JsonObject components;
    JsonObject location;

    // for later when setting values and states
    JsonObject channels;
};

#endif