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

// create a prototype:
OOCSIDevice::OOCSIDevice(const char* prototypeName, OOCSI &client, const char* OOCSIName) {
  oocsi = &client;
  oocsi->newMessage("heyOOCSI!");
  device = oocsi->addJsonObject(prototypeName);
  properties = device.createNestedObject("properties");
  properties["device_id"] = OOCSIName;
  components = device.createNestedObject("components");
  location = device.createNestedObject("location");
}

// Add device properties
void OOCSIDevice::addProperty(const char* property, const char* propertyValue) {
  properties[property] = propertyValue;
}

// Location initialisation
void OOCSIDevice::addLocation(const char* locationName, const char* latitude,const char* longitude) {
  location[locationName][0] = latitude;
  location[locationName][1] = longitude;
}

// Sensor configuration
void OOCSIDevice::addSensor(const char* sensorName, const char* sensorChannel, const char* sensorType, const char* sensorDefaultValue, const char* sensorUnit, const char* icon) {
  JsonObject sensor = components.createNestedObject(sensorName);
  sensor["channel_name"] = sensorChannel;
  sensor["type"] = "sensor";
  sensor["value"] = sensorDefaultValue;
  sensor["sensor_type"] = sensorType;
  sensor["unit"] = sensorUnit;
  sensor["icon"] = icon; 
}

// Number input configuration
void OOCSIDevice::addNumber(const char* numberName, const char* numberChannel, const char* numberMin, const char* numberMax, const char* numberDefaultValue, const char* numberUnit, const char* icon) {
  JsonObject number = components.createNestedObject(numberName);
  number["channel_name"] = numberChannel;
  number["type"] = "number";
  number["max"] = numberMax;
  number["min"] = numberMin;
  number["value"] = numberDefaultValue;
  number["sensor_type"] = "number";
  number["unit"]=numberUnit;
  number["icon"] = icon; 
}

// Binary sensor configuration
void OOCSIDevice::addBinarySensor(const char* sensorName, const char* sensorChannel, const char* sensorType, const char* sensorDefaultState, const char* icon) {
  JsonObject sensor = components.createNestedObject(sensorName);
  sensor["channel_name"] = sensorChannel;
  sensor["type"] = "binary_sensor";
  sensor["state"] = sensorDefaultState;
  sensor["sensor_type"] = sensorType;
  sensor["icon"] = icon; 
}

// Switch configuration
void OOCSIDevice::addSwitch(const char* switchName, const char* switchChannel, const char* switchDefaultState, const char* icon) {
  JsonObject switchConfig = components.createNestedObject(switchName);
  switchConfig["channel_name"] = switchChannel;
  switchConfig["type"] = "switch";
  switchConfig["state"] = switchDefaultState;
  switchConfig["icon"] = icon; 
}

// light configuration
void OOCSIDevice::addLight(const char* lightName, const char* lightChannel, const char* ledType, int spectrum, const char* lightDefaultState,  int defaultBrightness,  const char* icon) {
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
}

//light configuration incase of CCT lights
void OOCSIDevice::addLight(const char* lightName, const char* lightChannel, const char* ledType, int spectrum, const char* lightDefaultState,  int defaultBrightness, int miredMax, int miredMin, const char* icon) {
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
}

// send configured interview
void OOCSIDevice::sayHi() {
  oocsi->sendMessage();
}

// return OOCSIDevice with given name
OOCSIDevice OOCSI::heyOOCSI(const char* name) {
  return OOCSIDevice(name, *this, OOCSIName);
}