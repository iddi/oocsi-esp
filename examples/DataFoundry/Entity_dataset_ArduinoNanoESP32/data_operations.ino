bool addItem() {

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

  HttpClient http = HttpClient(wifi, host, HTTPS_PORT);  
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

  #ifdef ESP8266
  http.begin(wifi, address/*, root_ca_df*/);
  https.setAuthorization(root_ca_df);
  #else
  http.begin(address);
  #endif

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

bool updateItem() {

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

  HttpClient http = HttpClient(wifi, host, HTTPS_PORT);  
  http.beginRequest();
  http.put(address);
  http.sendHeader("Content-Type", F("application/json"));
  http.sendHeader("Content-Length", postMessage.length());
  http.sendHeader("resource_id", resource_id);
  http.sendHeader("token", resource_token);
  http.sendHeader("api_token", api_token);
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

  #ifdef ESP8266
  http.begin(wifi, address/*, root_ca_df*/);
  https.setAuthorization(root_ca_df);
  #else
  http.begin(address);
  #endif

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

bool deleteItem() {
  
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

  HttpClient http = HttpClient(wifi, host, HTTPS_PORT);  
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

  #ifdef ESP8266
  http.begin(wifi, address/*, root_ca_df*/);
  https.setAuthorization(root_ca_df);
  #else
  http.begin(address);
  #endif

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

bool getItem() {
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

  HttpClient http = HttpClient(wifi, host, HTTPS_PORT);  
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

  #ifdef ESP8266
  http.begin(wifi, address/*, root_ca_df*/);
  https.setAuthorization(root_ca_df);
  #else
  http.begin(address);
  #endif

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