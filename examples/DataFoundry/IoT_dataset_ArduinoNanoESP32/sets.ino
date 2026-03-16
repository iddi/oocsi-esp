// Initialize IoT dataset parameters for HTTPS request
void setDFDataset(const char* server, int id, const char* token) {
  host = server;
  dataset_id = id;
  api_token = token;
  logging = true;
  jsonMessage.clear();

  device_id = NULL;
  activity_id = NULL;
}

/*
 * Set methods for IoT dataset parameter
 */
// set device_id of sending device
void setDevice(const char* device) {
  device_id = device;
}

// set activity
void setActivity(const char* act) {
  activity_id = act;
}
