#include "OOCSIJsonParser.h"

boolean OOCSIJsonListener::has(String message, String key) {
	parser.setListener(this);
	parser.reset();

	this->thekey = "";
	for(int i = 0; i < message.length(); i++) {
		parser.parse(message.charAt(i));
		if(this->thekey == key) {
			return true;
		}
	}

	return false;
}

String OOCSIJsonListener::keys(String message) {
	parser.setListener(this);
	parser.reset();

	String found_keys = "";
	this->thekey = "";
	String current_key = "";
	for(int i = 0; i < message.length(); i++) {
		parser.parse(message.charAt(i));
		if(this->thekey != current_key) {
			if(this->thekey != "sender" && this->thekey != "recipient" && this->thekey != "timestamp") {
				found_keys.concat(this->thekey);
				found_keys.concat(',');
				current_key = this->thekey;
			}
		}
	}

	this->thevalue = "";
	return found_keys;
}

String OOCSIJsonListener::get(String message, String key) {
	parser.setListener(this);
	parser.reset();

	found_key = false;
	this->thekey = "";
	for(int i = 0; i < message.length(); i++) {
		parser.parse(message.charAt(i));

		if(!found_key && this->thekey == key) {
			found_key = true;
			this->thevalue = "";
			continue;
		}

		if(found_key && this->thevalue.length() > 0) {
			return this->thevalue;
		}
	}

	return "";
}

String OOCSIJsonListener::getArray(String message, String key) {
	parser.setListener(this);
	parser.reset();

	String result = "";
	found_key = false;
	this->thekey = "";
	for(int i = 0; i < message.length(); i++) {
		parser.setListener(this);
		parser.parse(message.charAt(i));

		if(!found_key && this->thekey == key) {
			found_key = true;
			this->thevalue = "";
			continue;
		}

		if(found_key && reading_array) {
			if(message.charAt(i) != '[') {
				result.concat(message.charAt(i));
			}
		}

		if(found_key && result.length() > 0 && !reading_array) {
			break;
		}
	}

	return result;
}

void OOCSIJsonListener::key(String key) {
  this->thekey = key;
}

void OOCSIJsonListener::value(String value) {
  if(found_key && !reading_array) {
    this->thevalue = value;
  }
}

void OOCSIJsonListener::startArray() {
  reading_array = true;
}

void OOCSIJsonListener::endArray() {
  reading_array = false;
}

void OOCSIJsonListener::endObject() {
}

void OOCSIJsonListener::startObject() {
}

void OOCSIJsonListener::whitespace(char c) {
}

void OOCSIJsonListener::startDocument() {
}

void OOCSIJsonListener::endDocument() {
}

