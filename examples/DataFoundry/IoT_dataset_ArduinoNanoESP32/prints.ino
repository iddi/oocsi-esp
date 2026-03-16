/*
 * Simplified print methods for easily print
 */
void print(const String &message) {
  if(logging)
    Serial.print(message);
}

void print(char message) {
  if(logging)
    Serial.print(message);
}

void println() {
  if(logging)
    Serial.println();
}

void println(const String &message) {
  if(logging)
    Serial.println(message);
}

void println(char message) {
  if(logging)
    Serial.println(message);
}