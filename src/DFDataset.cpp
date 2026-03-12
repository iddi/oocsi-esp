/***************************************************************************
 * The DataFoundry library for the ESP32, ESP8266, Arduino MKR Wifi 101,
 * Arduino UNO Wifi, Arduino Nano 33 IoT, and Arduino Nano RP2040
 * to store and retrieve data from ESPs and Arduinos on the Data
 * Foundry platform.
 *
 * Developed by Mathias Funk
 *
 * Attention: Arduino Nano ESP32 doesn't directly work with this library,
 *            please check another example: IoT_dataset_ArduinoNanoESP32
 * Last updated by Eden Chiang @ Mar. 12, 2026
 **************************************************************************/

#include "DFDataset.h"

#if defined(ESP8266) || defined(ESP32) || defined(NANO_ESP32) || defined(ARDUINO_UNOWIFIR4)
#include <time.h>
#endif

/**
 * @brief  Constructor for creating a new DFDataset
 * @note   Should only be called once
 * @retval A new DFDataset client
 */
DFDataset::DFDataset(const char* server, int id, const char* token) {
  host = server;
  dataset_id = id;
  api_token = token;
  logging = true;
  jsonMessage.clear();

  device_id = NULL;
  activity_id = NULL;
  resource_id = NULL;
  resource_token = NULL;
}

// iot datasets
void DFDataset::device(const char* device) {
  device_id = device;
}

void DFDataset::activity(const char* act) {
  activity_id = act;
}

// entity datasets
void DFDataset::id(const char* id) {
  resource_id = id;
}

void DFDataset::token(const char* token) {
  resource_token = token;
}

#if defined(ESP32) || defined(NANO_ESP32) || defined(ARDUINO_UNOWIFIR4)
static const char* root_ca_df =
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

#endif

#ifdef ESP8266
const char root_ca_8266[] = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

static BearSSL::X509List rootCA8266Cert(root_ca_8266);

#endif

// TLS connection configuration, not required for boards with NINA-W102 WiFi module
void DFDataset::ensureTlsConfigured() {
  static bool tls_ready = false;
  if (tls_ready) return;

#if defined(ESP8266) || defined(ESP32) || defined(NANO_ESP32)
  if (logging) println(F("Syncing time via NTP..."));

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 3600 * 24 * 365) delay(500);
  if (logging) println(F("Time synced."));

  #if defined(ESP8266)
  wifi.setTrustAnchors(&rootCA8266Cert);
  #else
  wifi.setCACert(root_ca_df);
  #endif

#elif defined(ARDUINO_UNOWIFIR4)
  wifi.setCACert(root_ca_df);
#else
  // WiFiNINA boards don't expose a setCACert function, they require seperate certificate flashing
#endif

  tls_ready = true;
}

bool DFDataset::logItem() {

  // check basic input data
  if(host == NULL || api_token == NULL || dataset_id == 0) {
    return false;
  }

  // check message
  String postMessage = String();
  serializeJson(jsonMessage, postMessage);
  if(postMessage == NULL || postMessage == "null") {
    return false;
  }

  // set missing data
  activity_id = activity_id != NULL ? activity_id : "";
  device_id = device_id != NULL ? device_id : "";

  // url encode activity id string because it will be part of the URL
  char activity_id_url[200];
  urlencode(activity_id_url, activity_id);

  // do transmission
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_UNOWIFIR4)
  // compile address
  snprintf_P(address, sizeof(address), PSTR("/datasets/ts/log/%i/%s"), dataset_id, activity_id_url);

  HttpClient http = HttpClient(wifi, host, 443);
  ensureTlsConfigured();
  http.beginRequest();
  http.post(address);
  http.sendHeader("Content-Type: application/json");
  http.sendHeader("api_token", api_token);
  http.sendHeader("source_id", device_id);
  http.sendHeader("device_id", device_id);
  http.sendHeader("Content-Length", postMessage.length());
  http.beginBody();
  http.print(postMessage);
  http.endRequest();

  int httpCode = http.responseStatusCode();
  // Serial.print("http code: ");
  // Serial.println(httpCode);

  // String response = http.responseBody();
  // Serial.print("Response: ");
  // Serial.println(response);
  jsonMessage.clear();

  return httpCode == 200;
#else
  // compile address
  snprintf_P(address, sizeof(address), PSTR("https://%s/datasets/ts/log/%i/%s"), host, dataset_id, activity_id_url);

  HTTPClient http;
  ensureTlsConfigured();

  http.begin(wifi, address);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("api_token", api_token);
  http.addHeader("source_id", device_id);
  http.addHeader("device_id", device_id);
  int httpCode = http.POST(postMessage);
  jsonMessage.clear();
  String response = http.getString();
  Serial.println("response: ");
  Serial.println(httpCode);

  http.end();

  return httpCode == 200;
#endif
}

bool DFDataset::addItem() {

  // check basic input data
  if(host == NULL || api_token == NULL || resource_id == NULL || dataset_id == 0) {
    return false;
  }

  // check message
  String postMessage = String();
  serializeJson(jsonMessage, postMessage);
  if(postMessage == NULL || postMessage == "null") {
    return false;
  }

  // set missing data
  resource_token = resource_token != NULL ? resource_token : "";

  // do transmission
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_UNOWIFIR4)
  // compile address
  snprintf_P(address, sizeof(address), PSTR("/datasets/entity/%i/item/"), dataset_id);

  HttpClient http = HttpClient(wifi, host, 443);
  ensureTlsConfigured();
  http.beginRequest();
  http.post(address);
  http.sendHeader("Content-Type: application/json");
  http.sendHeader("api_token", api_token);
  http.sendHeader("token", resource_token);
  http.sendHeader("resource_id", resource_id);
  http.sendHeader("Content-Length", postMessage.length());
  http.beginBody();
  http.print(postMessage);
  http.endRequest();
  
  int httpCode = http.responseStatusCode();
  jsonMessage.clear();

  return httpCode == 200;
#else
  // compile address
  snprintf_P(address, sizeof(address), PSTR("https://%s/datasets/entity/%i/item/"), host, dataset_id);

  HTTPClient http;
  ensureTlsConfigured();
  http.begin(wifi, address);

  http.addHeader("Content-Type", F("application/json"));
  http.addHeader("token", resource_token);
  http.addHeader("api_token", api_token);
  http.addHeader("resource_id", resource_id);

  int httpCode = http.POST(postMessage);
  jsonMessage.clear();

  http.end();

  return httpCode == 200;
#endif
}

bool DFDataset::updateItem() {

  // check basic input data
  if(host == NULL || api_token == NULL || resource_id == NULL || dataset_id == 0) {
    return false;
  }

  // check message
  String postMessage = String();
  serializeJson(jsonMessage, postMessage);
  if(postMessage == NULL || postMessage == "null") {
    return false;
  }

  // set missing data
  resource_token = resource_token != NULL ? resource_token : "";

  // do transmission
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_UNOWIFIR4)
  // compile address
  snprintf_P(address, sizeof(address), PSTR("/datasets/entity/%i/item/"), dataset_id);

  HttpClient http = HttpClient(wifi, host, 443);
  ensureTlsConfigured();
  http.beginRequest();
  http.put(address);
  http.sendHeader("Content-Type", F("application/json"));
  http.sendHeader("resource_id", resource_id);
  http.sendHeader("token", resource_token);
  http.sendHeader("api_token", api_token);
  http.sendHeader("Content-Length", postMessage.length());
  http.beginBody();
  http.print(postMessage);
  http.endRequest();

  int httpCode = http.responseStatusCode();
  jsonMessage.clear();

  return httpCode == 200;
#else
  // compile address
  snprintf_P(address, sizeof(address), PSTR("https://%s/datasets/entity/%i/item/"), host, dataset_id);

  HTTPClient http;
  ensureTlsConfigured();
  http.begin(wifi, address);

  http.addHeader("Content-Type", F("application/json"));
  http.addHeader("token", resource_token);
  http.addHeader("resource_id", resource_id);
  http.addHeader("api_token", api_token);

  int httpCode = http.PUT(postMessage);
  jsonMessage.clear();

  http.end();

  return httpCode == 200;
#endif
}

bool DFDataset::deleteItem() {
  
  // check basic input data
  if(host == NULL || api_token == NULL || resource_id == NULL || dataset_id == 0) {
    return false;
  }

  // set missing data
  resource_token = resource_token != NULL ? resource_token : "";

  // do transmission
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_UNOWIFIR4)
  // compile address
  snprintf_P(address, sizeof(address), PSTR("/datasets/entity/%i/item/"), dataset_id);

  HttpClient http = HttpClient(wifi, host, 443);
  ensureTlsConfigured();
  http.beginRequest();
  http.del(address);
  http.sendHeader("Content-Type", F("application/json"));
  http.sendHeader("resource_id", resource_id);
  http.sendHeader("token", resource_token);
  http.sendHeader("api_token", api_token);
  http.beginBody();
  http.print("");
  http.endRequest();
  
  int httpCode = http.responseStatusCode();
  jsonMessage.clear();

  return httpCode == 200;
#else
  // compile address
  snprintf_P(address, sizeof(address), PSTR("https://%s/datasets/entity/%i/item/"), host, dataset_id);

  HTTPClient http;
  ensureTlsConfigured();
  http.begin(wifi, address);

  http.addHeader("Content-Type", F("application/json"));
  http.addHeader("token", resource_token);
  http.addHeader("resource_id", resource_id);
  http.addHeader("api_token", api_token);

  int httpCode = http.sendRequest("DELETE");
  jsonMessage.clear();

  String jsonResponse = http.getString();

  http.end();

  return httpCode == 200;
#endif
}

bool DFDataset::getItem() {
  // check basic input data
  if(host == NULL || api_token == NULL || resource_id == NULL || dataset_id == 0) {
    return false;
  }

  // set missing data
  resource_token = resource_token != NULL ? resource_token : "";

  // do transmission
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_UNOWIFIR4)
  // compile address
  snprintf_P(address, sizeof(address), PSTR("/datasets/entity/%i/item/"), dataset_id);

  HttpClient http = HttpClient(wifi, host, 443);
  ensureTlsConfigured();
  http.beginRequest();
  http.get(address);
  http.sendHeader("Content-Type", F("application/json"));
  http.sendHeader("resource_id", resource_id);
  http.sendHeader("token", resource_token);
  http.sendHeader("api_token", api_token);
  http.beginBody();
  http.print("");
  http.endRequest();
  
  // int httpCode = http.responseStatusCode();
  String jsonResponse = http.responseBody();

  if(jsonResponse.length() > 0) {
    DeserializationError error = deserializeJson(jsonDocument, jsonResponse.c_str());
    if (error) {
      print(F("Message parsing failed: "));
      println(error.c_str());
      return false;
    }
    return true;
  } else {
    return false;
  }
#else
  // compile address
  snprintf_P(address, sizeof(address), PSTR("https://%s/datasets/entity/%i/item/"), host, dataset_id);

  HTTPClient http;
  ensureTlsConfigured();
  http.begin(wifi, address);

  http.addHeader("Content-Type", F("application/json"));
  http.addHeader("token", resource_token);
  http.addHeader("resource_id", resource_id);
  http.addHeader("api_token", api_token);

  /* int httpCode = */http.sendRequest("GET");
  jsonMessage.clear();

  String jsonResponse = http.getString();

  http.end();

  if(jsonResponse.length() > 0) {
    DeserializationError error = deserializeJson(jsonDocument, jsonResponse.c_str());
    if (error) {
      print(F("Message parsing failed: "));
      println(error.c_str());
      return false;
    }
    return true;
  } else {
    return false;
  }
#endif
}

bool DFDataset::getBool(const char* key, bool standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<bool>()) {
    return standard;
  } else {
    return jsonDocument[key].as<bool>();
  }
}

int DFDataset::getInt(const char* key, int standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<int>()) {
    return standard;
  } else {
    return jsonDocument[key].as<int>();
  }
}

long DFDataset::getLong(const char* key, long standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<long>()) {
    return standard;
  } else {
    return jsonDocument[key].as<long>();
  }
}

float DFDataset::getFloat(const char* key, float standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<float>()) {
    return standard;
  } else {
    return jsonDocument[key].as<float>();
  }
}

String DFDataset::getString(const char* key, const char* standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<String>()) {
    return standard;
  } else {
    return jsonDocument[key].as<String>();
  }
}

bool DFDataset::has(const char* key) {
  return jsonDocument[key].is<JsonVariant>();
}

// function for outputting all (top-level) keys in the message as a comma-separated list
String DFDataset::keys() {
  if(jsonDocument.isNull() || jsonDocument.size() == 0) {
    return "";
  }

  // comma separated key list
  String result = "";
  for (JsonPair p : jsonDocument.as<JsonObject>()) {
      if(result.length() > 0) {
        result.concat(",");  
      }
      result.concat(p.key().c_str());
  }
  return result;
}

// function for sending a bool to DFDataset
DFDataset DFDataset::addBool(const char* key, bool value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending an int to DFDataset
DFDataset DFDataset::addInt(const char* key, int value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending an int to DFDataset
DFDataset DFDataset::addLong(const char* key, long value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending a float to DFDataset
DFDataset DFDataset::addFloat(const char* key, float value) {
  jsonMessage[key] = value;
  return *this;
}

DFDataset DFDataset::addString(const char* key, const char* value) {
  jsonMessage[key] = value;
  return *this;
}

void DFDataset::print(const String &message) {
  if(logging)
    Serial.print(message);
}

void DFDataset::print(char message) {
  if(logging)
    Serial.print(message);
}

void DFDataset::println() {
  if(logging)
    Serial.println();
}

void DFDataset::println(const String &message) {
  if(logging)
    Serial.println(message);
}

void DFDataset::println(char message) {
  if(logging)
    Serial.println(message);
}

void DFDataset::setActivityLEDPin(int ledPin) {
  activityLEDPin = ledPin;
}

void DFDataset::setLogging(bool log) {
  logging = log;
}

void DFDataset::urlencode(char* dst, const char* src)
{
  char encodedString[200] = "";
  char c;
  char code0;
  char code1;

  for (int i =0; i < strlen(src); i++){
    c=src[i];
    if (isalnum(c)){
      sprintf(encodedString, "%s%c", encodedString, c);
    } else {
      code1=(c & 0xf)+'0';
      if ((c & 0xf) >9){
          code1=(c & 0xf) - 10 + 'A';
      }
      c=(c>>4)&0xf;
      code0=c+'0';
      if (c > 9){
          code0=c - 10 + 'A';
      }
      sprintf(encodedString, "%s%c", encodedString, '%');
      sprintf(encodedString, "%s%c", encodedString, code0);
      sprintf(encodedString, "%s%c", encodedString, code1);
    }
    yield();
  }

  strcpy(dst, encodedString);
}
