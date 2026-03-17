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
  // for Arduino Uno R4 WiFi and other boards with WiFININA module
  // compile address
  snprintf_P(address, sizeof(address), PSTR("/datasets/ts/log/%i/%s"), dataset_id, activity_id_url);

  // create https request
  HttpClient https = HttpClient(wifi, host, HTTPS_PORT);
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