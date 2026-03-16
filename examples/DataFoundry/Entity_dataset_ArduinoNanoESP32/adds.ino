// function for sending a bool to DFDataset
void addBool(const char* key, bool value) {
  jsonMessage[key] = value;
  // return *this;
}

// function for sending an int to DFDataset
void addInt(const char* key, int value) {
  jsonMessage[key] = value;
  // return *this;
}

// function for sending an int to DFDataset
void addLong(const char* key, long value) {
  jsonMessage[key] = value;
  // return *this;
}

// function for sending a float to DFDataset
void addFloat(const char* key, float value) {
  jsonMessage[key] = value;
  // return *this;
}

void addString(const char* key, const char* value) {
  jsonMessage[key] = value;
  // return *this;
}
