void printByteAsBool(uint8_t b) {
  Serial.print(b ? "True" : "False");
}

void printByteAsHex(uint8_t b) {
  Serial.print("0x");
  if (b < 16) {
    Serial.print("0");
  }
  Serial.print(b, HEX);
}

void printWordAsHex(uint16_t w) {
  Serial.print("0x");
  if (w < 16) {
    Serial.print("000");
  } else if (w < 256) {
    Serial.print("00");
  } else if (w < 4096) {
    Serial.print("0");
  }
  Serial.print(w, HEX);
}
