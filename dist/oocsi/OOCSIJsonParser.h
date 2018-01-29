#ifndef OOCSI_JSON_PARSER_h
#define OOCSI_JSON_PARSER_h

#include "JsonListener.h"
#include "JsonStreamingParser.h"

class OOCSIJsonListener: public JsonListener {

  public:

  	boolean has(String message, String key);
  	String keys(String message);
    String get(String message, String key);
    String getArray(String message, String key);

    void key(String key);
    void value(String value);

    void startArray();
    void startObject();
    void endArray();
    void endObject();

    void whitespace(char c);
    void startDocument();
    void endDocument();

  private:

    String thekey;
    boolean found_key;
    boolean reading_array;
	  String thevalue;
	  JsonStreamingParser parser;

};

#endif