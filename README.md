# OOCSI for ESP

The OOCSI mission is to create a design middleware for use by designers and creative technologist. OOCSI allows to prototype with connectivity.

There are two basic components that form an OOCSI network: the client and the server. While the server can be started from the command line (see here), the client interface need to be embedded in other code and allows this code to access the OOCSI network, to send and receive data in a simple way.

The OOCSI for ESP library is made for the Arduino IDE and it allows to directly use most of the OOCSI functionality in small-scale systems. We tested this library with the ESP32 and ESP8266.

# How to install
To install the library, download the library archive **oocsi.zip** from the latest [release](https://github.com/iddi/oocsi-esp/releases) and unpack **oocsi.zip** into the libraries folder of your Arduino IDE (usually here: Documents/Arduino/libraries/). After this, restart the Arduino IDE and check whether a new examples category "OOCSI" is shown. If yes, all fine, start coding (jump to 'How to use'). 

In case this does not work yet, check again that the library is in the right folder. Make sure that you have downloaded the `oocsi.zip` library archive and *not* the source archive of the release. If the library shows up in a subfolder 'INCOMPATIBLE', you need to first select a compatible board in the Tools>Board menu of the Arduino IDE. Select either an ESP32 or ESP8266 board, install the boards via the 'Boards Manager' if you cannot find any ESP32 or ESP8266 boards listed.

**Important notice from version 1.4.0**
The library "ArduinoJson" is *required* by the OOCSI library from OOCSI for ESP version 1.4.0. Follow the [step-by-step guide](https://arduinojson.org/v6/doc/installation/) to install it in your Arduino IDE. Please ensure that you install version 6 or higher of "ArduinoJson".

# How to use
To use OOCSI for ESP, please check out the example code that is packaged with the library. Below, the different functions of the library will be explained divided into the following parts: connecting to a network, sending data to the network and receiving data from the network. We will use similar code as in the library examples.

## Connecting
To connect to an OOCSI network, that is, a running [OOCSI server](https://github.com/iddi/oocsi/releases), you first need to create an OOCSI reference: 

	OOCSI oocsi = OOCSI();

This will not yet connect to the network. To connect, you have to use the *connect* function which takes multiple parameters: the unique handle of the client in the OOCSI network (this is important to identify itself and receive direct messages), the address of the OOCSI server, the name and password of the Wifi network to use and a reference to a function that will be called to process incoming messages.

	oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
	...
	void processOOCSI() {
		// event data processing here...
	}

When running this code, the library will start by connecting to the Wifi *ssid* and, if that is successful, then to the OOCSI server at *hostserver*. The progress will be logged on the Serial console. To better understand how to process incoming messages, refer to the _Receiving data_ section below.


## Sending data
Sending data to one or more recipients in the OOCSI network works in three steps: creating a new message, adding data to the message, and sending the message out.

### Creating a new message
A new message is always created for a channel (broadcast) or the handle of another client (direct message). In the example below, the new message is created to be sent to the channel "testchannel", that is, to all clients subscribing to that channel (see below how to do that).

	oocsi.newMessage("testchannel");
	
After this command, data can be added to the message. Please note that the ESP library can currently only work with one outgoing message at a time, which should be fine for more use-cases.

### Adding data to a message
A message is empty at first. While it is certainly possible to send an empty message (for instance, as a "heartbeat" or "ping" signal), adding data makes it more useful. A message in OOCSI consists of several pairs of each a key and a value. The *key* identifies and labels the data. With the key the recipient of the message can easily retrieve it. The *value* that is given with the key, can be of many types. In this library, only a few basic types are implemented, which should cover 95% of all cases: int, long, float, and string.

	oocsi.addInt("counter", 40)
	oocsi.addLong("timestamp", 5000000);
	oocsi.addFloat("angle", 50.6);
	oocsi.addString("message", "value as string");

It is also possible to _chain_ the commands to add data:

	oocsi.addInt("counter", 40).addLong("timestamp", (long) 51250220);


### Sending the message
After the message is complete, we can send it out:

	oocsi.sendMessage();

Again, it is possible to chain all commands for sending a message:

	oocsi.newMessage("testchannel").addInt("counter", 6).sendMessage();
  
To see what was actually sent out, you can use the printMessage command:

	oocsi.printSendMessage();


## Receiving data
Receiving data from OOCSI works with a processing function that is called whenever new data has arrived. This function is referenced in the beginning when connecting to the network.

	void processOOCSI() {
		// event data processing here...
	}

### Subscribing to a channel
If we just want to receive direct messages, we do not need to subscribe to a channel. Nevertheless, it is good practice to send data via a channel, so others can subscribe and make use of my data. Subscribing to ta channel is straight-forward:

	oocsi.subscribe("testchannel");

We can subscribe to as many channels as we want; if data is sent to these channels, we receive it. See below for receiving messages from mutliple channels.

### Getting data out
There are similar functions to adding data, for getting out. Commonly, the data type is identified by the command, the first parameter provides the *key* and the last parameter provides a *default value* that is used in case the key could not be found in the message. In the examples below we use -200 as the default value (anything is possible actually).

	int ivalue = oocsi.getInt("counter", -200);
	float fvalue = oocsi.getFloat("angle", -200.0);
	long lvalue = (long) oocsi.getInt("timestamp", -200);
	string svalue = oocsi.getString("message", "-200");

There is an exception for the *long* data type, where we use getInt and cast to long.

### Getting message meta-data
A message always contains meta-data, the sender handle, the recipient which is either a channel or the unique handle of the receiving client and a timestamp.

	string sender = oocsi.getSender();
	string recipient = oocsi.getRecipient();
	long timestamp = oocsi.getTimeStamp();

Before using a key, we can also check whether the message contains data with the given key:

	if(oocsi.has("counter")) {
		// get data out
	}


### Receiving data from multiple channels and directly
Sometimes we would like to subscribe to multiple channels at the same time. All messages will arrive via the processing function that we have registered before. We can use the message recipient to distinguish between message coming from different channels (or directly).

	void processOOCSI() {
		// event data processing here...
		if (oocsi.getRecipient() == "channel1") {
			// extract data for this message from channel "channel1"
		} else if (oocsi.getRecipient() == "channel2") {
			// extract data for this message from channel "channel2"
		}
	}

If we have registered the client with a specific handle when connecting to OOCSI, we can also compare with this handle to filter messages that were sent directly to this client only.

	if (oocsi.getRecipient() == "myhandle") {
		// extract data for this direct message
	}


## Other functions
There are more functions in the OOCSI for ESP library. There will be explained here briefly.

### Debugging
The library will log its output to Serial. If this is too noisy, we can switch OOCSI for ESP to silently mode: 

	oocsi.setLogging(false);

To switch logging on again, use: 

	oocsi.setLogging(true);

### Activity LED
The library can use an LED to signal activity. Use this by switching the respective pin to OUTPUT and tell the library about which pin to use: 

	pinMode(5, OUTPUT);
	oocsi.setActivityLEDPin(5);

  
## Examples
We have included several examples in the OOCSI for ESP library. The most commonly used examples are listed directly in the OOCSI examples category. You will also find a 'Data Foundry' subcategory which contains examples that show how to use the [Data Foundry](https://data.id.tue.nl) with ESP32 and ESP8266 boards. The 'Extras' subcategory contains examples that explain how to work with random client names, how to retrieve time information from OOCSI, how to work with multiple WiFis or even a freely configurable WiFi manager.


## Credits
Initial development: Jort Band
Stable version and maintenance: Mathias Funk
Testing and support: Henk Apeldoorn, Geert van den Boomen, Dirk van de Mortel

Find the OOCSI mothership project at https://github.com/iddi/oocsi

---

# Compatibility for versions < 1.4.x
| Module | Arduino board name | ESP variant | v1.2.1 | v1.3.1 |
| ---- |:----:|:----:|:----:| :----: |
| Sparkfun ESP32 Thing | ESP32 DEV Module | 32 | OK | OK | 
| Sparkfun ESP32 Thing | Sparkfun ESP32 Thing | 32 | OK | OK | 
| Sparkfun ESP8266 Thing | Sparkfun ESP8266 Thing | 8266 | OK | UR | 
| WeMos D1 mini | LOLIN(WEMOS) D1 R2 & mini | 8266 | OK | UR  |
| WeMos D1 v3.0.0 | LOLIN(WEMOS) D1 R2 & mini | 8266 | OK | UR |
| WeMos D1 v3.0.0 | LOLIN(WEMOS) D1 mini Lite | 8266 | OK | UR |
| NodeMCU (AI-Thinker) | NodeMCU 1.0 | 8266 | OK | OK |
| NodeMCU (AI-Thinker) | NodeMCU 0.9 | 8266 |  -  |  -  |
| Adafruit Feather HUZZAH | Adafruit Feather HUZZAH ESP8266 | 8266 | UR | - |

*OK = sending / receiving all fine; UR = unstable run, might crash*

Check the table above for compatilibity with different ESPs. Version v1.2.1 is more basic and can only deal with first level event data. v1.3.1 can parse JSON properly, but needs more memory, making it less stable on some low-memory modules. Both versions are available from https://github.com/iddi/oocsi-esp/releases. If you are not sure which module to get, go for the ESP32!


