/*
 * Methods to check the keys of data / message
 */

bool hasKey(const char* key) {
  return jsonDocument[key].is<JsonVariant>();
}

// function for outputting all (top-level) keys in the message as a comma-separated list
String getKeys() {
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