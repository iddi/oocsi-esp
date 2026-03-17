// function for sending an int to DFDataset
void addInt(const char* key, int value) {
  jsonMessage[key] = value;
}

// function for sending a string to DFDataset
void addString(const char* key, const char* value) {
  jsonMessage[key] = value;
}

// function for sending a boolean to DFDataset
void addBool(const char* key, bool value) {
  jsonMessage[key] = value;
}

// function for sending a long number to DFDataset, from -2147483648 to 2147483647.
void addLong(const char* key, long value) {
  jsonMessage[key] = value;
}

// function for sending a float to DFDataset
void addFloat(const char* key, float value) {
  jsonMessage[key] = value;
}