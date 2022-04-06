/***************************************************************************
 * The OOCSIDevice library for the ESP32, ESP8266, Arduino MKR Wifi 101, 
 * Arduino UNO Wifi, Arduino Nano 33 IoT, and Arduino Nano RP2040 to 
 * connect to the OOCSI platform (https://github.com/iddi/oocsi).
 * It allows to register ESP and Arduino-based OOCSI clients as annotated 
 * devices on the OOCSI platform (and HomeAssistant).
 *
 * Developed by Diede van Marle, Mathias Funk
 **************************************************************************/

#include "OOCSIDevice.h"

// create a device for a given OOCSI client
OOCSIDevice::OOCSIDevice(const char* name, OOCSI &client) {
  oocsi = &client;
  prototypeName = name;

  StaticJsonDocument<1000> docDevices;
  device = docDevices.to<JsonObject>();
  properties = device.createNestedObject("properties");
  components = device.createNestedObject("components");
  location = device.createNestedObject("location");

  StaticJsonDocument<1000> docChannels;
  channels = docChannels.to<JsonObject>();
}

// add device properties
void OOCSIDevice::addProperty(const char* property, const char* propertyValue) {
  properties[property] = propertyValue;
}

// location initialisation
void OOCSIDevice::addLocation(const char* locationName, int latitude, int longitude) {
  location[locationName][0] = latitude;
  location[locationName][1] = longitude;
}

// sensor configuration
void OOCSIDevice::addSensor(const char* sensorName, const char* sensorChannel, const char* sensorType, int sensorDefaultValue, const char* sensorUnit, const char* icon) {
  JsonObject sensor = components.createNestedObject(sensorName);
  sensor["channel_name"] = sensorChannel;
  sensor["type"] = "sensor";
  sensor["value"] = sensorDefaultValue;
  sensor["sensor_type"] = sensorType;
  sensor["unit"] = sensorUnit;
  sensor["icon"] = icon;
  channels[sensorName] = sensorChannel;
}

// number input configuration
void OOCSIDevice::addNumber(const char* numberName, const char* numberChannel, float numberMin, float numberMax, float numberDefaultValue, const char* numberUnit, const char* icon) {
  JsonObject number = components.createNestedObject(numberName);
  number["channel_name"] = numberChannel;
  number["type"] = "number";
  number["max"] = numberMax;
  number["min"] = numberMin;
  number["value"] = numberDefaultValue;
  number["sensor_type"] = "number";
  number["unit"]=numberUnit;
  number["icon"] = icon;
  channels[numberName] = numberChannel;
}

// number input configuration
void OOCSIDevice::addNumber(const char* numberName, const char* numberChannel, float numberDefaultValue, const char* numberUnit, const char* icon) {
  JsonObject number = components.createNestedObject(numberName);
  number["channel_name"] = numberChannel;
  number["type"] = "number";
  number["value"] = numberDefaultValue;
  number["sensor_type"] = "number";
  number["unit"]=numberUnit;
  number["icon"] = icon;
  channels[numberName] = numberChannel;
}

// binary sensor configuration
void OOCSIDevice::addBinarySensor(const char* sensorName, const char* sensorChannel, const char* sensorType, bool sensorDefaultState, const char* icon) {
  JsonObject sensor = components.createNestedObject(sensorName);
  sensor["channel_name"] = sensorChannel;
  sensor["type"] = "binary_sensor";
  sensor["state"] = sensorDefaultState;
  sensor["sensor_type"] = sensorType;
  sensor["icon"] = icon;
  channels[sensorName] = sensorChannel;
}

// switch configuration
void OOCSIDevice::addSwitch(const char* switchName, const char* switchChannel, bool switchDefaultState, const char* icon) {
  JsonObject switchConfig = components.createNestedObject(switchName);
  switchConfig["channel_name"] = switchChannel;
  switchConfig["type"] = "switch";
  switchConfig["state"] = switchDefaultState;
  switchConfig["icon"] = icon;
  channels[switchName] = switchChannel;
}

// light configuration
void OOCSIDevice::addLight(const char* lightName, const char* lightChannel, const char* ledType, int spectrum, bool lightDefaultState, int defaultBrightness, const char* icon) {
  JsonObject light = components.createNestedObject(lightName);
  light["channel_name"] = lightChannel;
  light["type"] = "light";
  light["state"] = lightDefaultState;
  light["ledType"] = ledType;
  light["brightness"] = defaultBrightness;
  light["icon"] = icon; 
  JsonArray spectrums = light.createNestedArray("spectrum");
  switch(spectrum) {
  	case 1:
  	case 4:
  	case 6:
  	case 7:
      spectrums.add("RGB");
      break;
  }
  switch(spectrum) {
  	case 2:
  	case 4:
  	case 5:
  	case 7:
      spectrums.add("CCT");
      break;
  }
  switch(spectrum) {
  	case 3:
  	case 5:
  	case 6:
  	case 7:
      spectrums.add("WHITE");
      break;
  }
  channels[lightName] = lightChannel;
}

// light configuration incase of CCT lights
void OOCSIDevice::addLight(const char* lightName, const char* lightChannel, const char* ledType, int spectrum, bool lightDefaultState,  int defaultBrightness, int miredMax, int miredMin, const char* icon) {
  JsonObject light = components.createNestedObject(lightName);
  light["channel_name"] = lightChannel;
  light["type"] = "light";
  light["state"] = lightDefaultState;
  light["ledType"] = ledType;
  light["brightness"] = defaultBrightness;
  light["icon"] = icon; 
  JsonArray spectrums = light.createNestedArray("spectrum");
  switch(spectrum) {
  	case 1:
  	case 4:
  	case 6:
  	case 7:
      spectrums.add("RGB");
      break;
  }
  switch(spectrum) {
  	case 2:
  	case 4:
  	case 5:
  	case 7:
      spectrums.add("CCT");
      break;
  }
  switch(spectrum) {
  	case 3:
  	case 5:
  	case 6:
  	case 7:
      spectrums.add("WHITE");
      break;
  }
  light["max"] = miredMax;
  light["min"] = miredMin;
  channels[lightName] = lightChannel;
}

// send configured interview
void OOCSIDevice::sayHi() {
  oocsi->newMessage("heyOOCSI!");
  String name = oocsi->getName();
  properties["device_id"] = name;
  if(strlen(prototypeName) == 0) {
    oocsi->setJsonObject(name.c_str(), device);
  } else {
    oocsi->setJsonObject(prototypeName, device);
  }
  oocsi->sendMessage();
}

// --------------------------------------------------------------------------------------------------------------------

void OOCSIDevice::setValue(const char* componentName, float value) {
  setValue(componentName, "value", value);
}

void OOCSIDevice::setValue(const char* componentName, const char* key, float value) {
  if(channels.containsKey(componentName)) {
    oocsi->newMessage(channels[componentName]);
    oocsi->addFloat(key, value);
    oocsi->addString("component", componentName);
    oocsi->sendMessage();
  }
}

void OOCSIDevice::setState(const char* componentName, bool state) {
  setState(componentName, "value", state);
}

void OOCSIDevice::setState(const char* componentName, const char* key, bool state) {
  if(channels.containsKey(componentName)) {
    oocsi->newMessage(channels[componentName]);
    oocsi->addBool(key, state);
    oocsi->addString("component", componentName);
    oocsi->sendMessage();
  }
}

// --------------------------------------------------------------------------------------------------------------------

// return OOCSIDevice with default name
OOCSIDevice OOCSI::heyOOCSI() {
  return OOCSIDevice("", *this);
}

// return OOCSIDevice with given name
OOCSIDevice OOCSI::heyOOCSI(const char* name) {
  return OOCSIDevice(name, *this);
}