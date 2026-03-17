/*
 * Methods for reading the value of a specific key from data
 */

bool getBool(const char* key, bool standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<bool>()) {
    return standard;
  } else {
    return jsonDocument[key].as<bool>();
  }
}

int getInt(const char* key, int standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<int>()) {
    return standard;
  } else {
    return jsonDocument[key].as<int>();
  }
}

long getLong(const char* key, long standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<long>()) {
    return standard;
  } else {
    return jsonDocument[key].as<long>();
  }
}

float getFloat(const char* key, float standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<float>()) {
    return standard;
  } else {
    return jsonDocument[key].as<float>();
  }
}

String getString(const char* key, const char* standard) {
  if(jsonDocument.isNull() || !jsonDocument[key].is<String>()) {
    return standard;
  } else {
    return jsonDocument[key].as<String>();
  }
}