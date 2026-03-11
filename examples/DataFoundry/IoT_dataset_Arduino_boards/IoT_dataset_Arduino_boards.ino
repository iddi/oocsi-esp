/*
 *     Description: This example code is for people who is logging data via HTTPS POST 
 *                  requests with the token of IoT dataset in Data Foundry. 
 *    Target model: Arduino Nano ESP32, Arduino Uno R4 WiFi. 
 *                  And other models with Espressif ESP32 family chips or ESP8266 chip 
 *                  should also work fine.
 *                  For boards with the NINA-W102 Wifi module built-in, upload SSL
 *                  certificate separately is required, official reference:
 *                  https://docs.arduino.cc/software/ide-v2/tutorials/ide-v2-fw-cert-uploader/
 *  Target dataset: IoT dataset in Data Foundry
 *            Date: Mar. 11, 2026
 *          Author: I-Tang (Eden) Chiang
 */

#include <time.h>

#if defined(ESP32) || defined(NANO_ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#endif

#ifdef ARDUINO_UNOWIFIR4
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#endif

#if defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_SAMD_NANO_33_IOT)
// boards with NINA-W102 WiFi module, correct certification pre-upload is required
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#endif

#include <ArduinoJson.h>

// --- Wifi network setup ---
const char* ssid = "SSID_OF_WIFI_NETWORK";
// Password of your Wifi network.
const char* password = "PASSWORK_OF_WIFI_NETWORK";

// the server address of your Data Foundry instance (server of TU/e)
const char* datafoundry = "data.id.tue.nl";

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

// WiFi setting
#if defined(ESP8266) || defined(ESP32)
WiFiClientSecure wifi;
#else
WiFiSSLClient wifi;
#endif

// Certification setup for HTTPS protocol
const char root_ca[] = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

const char* root_ca_df =
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

#ifdef ESP8266
static BearSSL::X509List rootCACert(root_ca);
#endif

// setup for HTTPS request
void setDFDataset(const char* server, int id, const char* token) {
  host = server;
  dataset_id = id;
  api_token = token;
  logging = true;
  jsonMessage.clear();

  device_id = NULL;
  activity_id = NULL;
}

// set device_id of sending device
void setDevice(const char* device) {
  device_id = device;
}

// set activity
void setActivity(const char* act) {
  activity_id = act;
}

// quick print
void print(const String &message) {
  if(logging)
    Serial.print(message);
}

// quick print
void print(char message) {
  if(logging)
    Serial.print(message);
}

// quick print
void println() {
  if(logging)
    Serial.println();
}

// quick print
void println(const String &message) {
  if(logging)
    Serial.println(message);
}

// quick print
void println(char message) {
  if(logging)
    Serial.println(message);
}

// URL encoder
void urlencode(char* dst, const char* src) {
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

// function for sending an int to DFDataset
void addInt(const char* key, int value) {
  jsonMessage[key] = value;
}

// function for sending a string to DFDataset
void addString(const char* key, const char* value) {
  jsonMessage[key] = value;
}

// function for sending a boolean to DFDataset
void addBool(const char* key, bool value) {
  jsonMessage[key] = value;
}

// function for sending a long number to DFDataset, from -2147483648 to 2147483647.
void addLong(const char* key, long value) {
  jsonMessage[key] = value;
}

// function for sending a float to DFDataset
void addFloat(const char* key, float value) {
  jsonMessage[key] = value;
}

// TLS connection configuration, not required for boards with NINA-W102 WiFi module
void ensureTlsConfigured() {
  static bool tls_ready = false;
  if (tls_ready) return;

#if defined(ESP8266) || defined(ESP32)
  if (logging) println(F("Syncing time via NTP..."));

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 3600 * 24 * 365) delay(500);
  if (logging) println(F("Time synced."));

  #if defined(ESP8266)
    wifi.setTrustAnchors(&rootCACert);
  #else
    wifi.setCACert(root_ca_df);
  #endif

#elif defined(ARDUINO_UNOWIFIR4)
  wifi.setCACert(root_ca_df);
#else
  // WiFiNINA boards, upload certification in advance is required
#endif

  tls_ready = true;
}

// logging data to Data Foundry
bool logItem() {
  Serial.println("logitem");
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
#if defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_SAMD_NANO_33_IOT)
  // for Arduino Uno R4 WiFi and other boards with NINA WiFI module
  // compile address
  snprintf_P(address, sizeof(address), PSTR("/datasets/ts/log/%i/%s"), dataset_id, activity_id_url);

  // create https request
  HttpClient https = HttpClient(wifi, host, 443);
  ensureTlsConfigured();
  https.beginRequest();
  https.post(address);

  // setup headers
  https.sendHeader("Content-Type: application/json");
  https.sendHeader("api_token", api_token);
  https.sendHeader("device_id", device_id);
  https.sendHeader("Content-Length", postMessage.length());

  // setup data
  https.beginBody();
  https.print(postMessage);

  // close request
  https.endRequest();
  
  // code of server response
  int httpCode = https.responseStatusCode();

  // reset message object
  jsonMessage.clear();

  // response check, can be comment out as of 200 is returned
  print("http code: ");
  Serial.println(httpCode);

  return httpCode == 200;
#else
  // for Arduino Nano ESP32, ESP32, and ESP8266
  // compile address
  snprintf_P(address, sizeof(address), PSTR("https://%s/datasets/ts/log/%i/%s"), host, dataset_id, activity_id_url);
  
  // setup https request
  HTTPClient https;
  ensureTlsConfigured();
  https.begin(wifi, address);

  // setup headers
  https.addHeader("Content-Type", "application/json");
  https.addHeader("api_token", api_token);
  https.addHeader("device_id", device_id);
  int httpCode = https.POST(postMessage);
  jsonMessage.clear();

  // response check, can be comment out as of 200 is returned
  print("http code: ");
  Serial.println(httpCode);

  https.end();

  return httpCode == 200;
#endif
}

void setup() {
  // init serial port
  Serial.begin(115200);

  // create Wifi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // create connection to dataset with server address, dataset id, and the access token
  setDFDataset(datafoundry, DATASET_ID, "TOKEN_OF_IoT_DATASET_FOR_WEB_ACCESS");
}

void loop() {
  // specify device, can be empty
  setDevice("RefID_of_device_in_DataFoundry");

  // specify activity, can be empty
  setActivity("idle");

  // fill in some data for the item (very similar to OOCSI)
  addInt("numericalTimingData", millis());
  addBool("yesno", true);
  addString("stringData", "a long, long, long, long, long string - not really, but you get the point.");
  addString("dev_id", "d54d34c1bd3f241c8");
  addFloat("temp", 33.12);
  addLong("MAX-long", 2147483647L);

  logItem();
  delay(5000);
}