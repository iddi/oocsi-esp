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
  // NINA-W102 boards, upload certificate in advance is required
#endif

  tls_ready = true;
}

// URL encoder for POST request
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
