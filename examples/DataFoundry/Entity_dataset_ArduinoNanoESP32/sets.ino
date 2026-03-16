// Initialize entity dataset parameters for HTTPS request
void setDFDataset(const char* server, int id, const char* token) {
  host = server;
  dataset_id = id;
  api_token = token;
  logging = true;
  jsonMessage.clear();

  device_id = NULL;
  activity_id = NULL;
}

// Set methods for Entity dataset parameter
void setId(const char* id) {
  resource_id = id;
}

void setToken(const char* token) {
  resource_token = token;
}