#include <OOCSI.h>
#include "DHT.h"

// Example testing sketch for various DHT humidity/temperature sensors written by ladyada, modified by Eva van der Born
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - OOCSI for ESP Library: https://github.com/iddi/oocsi-esp

// Digital pin connected to the DHT sensor
#define DHTPIN 4
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22 (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

DHT dht(DHTPIN, DHTTYPE);

// FOR OOCSI CONNECTION: 
// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "yourssid";
// Password of your Wifi network.
const char* password = "yourpassword";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_OOCSI_CLIENT_SENDER";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "your OOCSI server address";

// OOCSI reference for the entire sketch
// To connect to an OOCSI network, that is, a running OOCSI server, you first need to create an OOCSI reference:
OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();
   
  // output OOCSI activity on the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  oocsi.setActivityLEDPin(LED_BUILTIN);
  oocsi.connect(OOCSIName, hostserver, ssid, password);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float fahrenheit_temperature = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float heat_index_fahrenheit = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float heat_index_celsius = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  // make OOCSI message ready. Insert DF channel here
  oocsi.newMessage("CHANNEL");

  // add data (primitive data types int, float, long, string)
  // the labels such as "count" or "timestamp" are completely free to choose

  // replace DEVICE with your reference ID from a device
  oocsi.addString("device_id", "DEVICE");
  
  oocsi.addFloat("Humidity", humidity);
  oocsi.addFloat("Humidity", temperature);
  oocsi.addFloat("Humidity", heat_index_celsius);
  oocsi.addLong("time", (long) millis());

  // this command will send the message; don't forget to call this after creating a message
  oocsi.sendMessage();
  
  // prints out the raw message (how it is sent to the OOCSI server)
  // oocsi.printSendMessage();
  // needs to be checked in order for OOCSI to process incoming data.
  oocsi.check();
}
