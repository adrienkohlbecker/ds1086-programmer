String i2c_error(uint8_t err) {
  switch (err)
  {
      case 1:
        return "data too long to fit in transmit buffer";
        break;
      case 2:
        return "received NACK on transmit of address";
        break;
      case 3:
        return "received NACK on transmit of data";
        break;
      case 4:
        return "other error";
        break;
      case 5:
        return "timeout";
        break;
      default:
        Serial.print("Error: Invalid i2c error ");
        Serial.print(err);
        Serial.println(", expected 1 to 5");
        return "invalid i2c error";
  }
}

bool i2c_read(uint8_t address, uint8_t reg, uint8_t retval[], uint8_t count) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  uint8_t status = Wire.endTransmission(false);    // Do not release the line
  if (status != 0) {
    Serial.print("Error during i2c_read: ");
    Serial.print(i2c_error(status));
    Serial.print(". Wrote ");
    printByteAsHex(reg);
    Serial.print(" to ");
    printByteAsHex(address);
    Serial.println();
    return false;
  }

  uint8_t n = Wire.requestFrom(address, count);
  if (n < count) {
    Serial.print("Error during i2c_read: device did not send expected number of bytes: ");
    Serial.println(count);
    return false;
  }

  for (uint8_t i=0; i<count; i++) {
    retval[i] = Wire.read();
  }

  return true;
}

uint8_t i2c_read_byte(uint8_t address, uint8_t reg) {
  uint8_t res[1];
  bool success = i2c_read(address, reg, res, 1);
  if (!success) {
    return 0;
  }

  return res[0];
}

uint16_t i2c_read_word(uint8_t address, uint8_t reg) {
  uint8_t res[2];
  bool success = i2c_read(address, reg, res, 2);
  if (!success) {
    return 0;
  }

  return uint16_t(res[1]) << 8 | res[0];
}

void i2c_write(uint8_t address, uint8_t reg, uint8_t data[], uint8_t len) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data, len);

  int status = Wire.endTransmission();
  if (status != 0) {
    Serial.print("Error during i2c_write: ");
    Serial.print(i2c_error(status));
    Serial.print(". Wrote ");
    printByteAsHex(reg);
    Serial.print("and ");
    for (uint8_t i=0; i<len; i++) {
      printByteAsHex(data[i]);
      Serial.print(" ");
    }
    Serial.print("to ");
    printByteAsHex(address);
    Serial.println();
    return;
  }
}

void i2c_write_void(uint8_t address, uint8_t reg) {
  uint8_t b[0] = {};
  i2c_write(address, reg, b, 0);
}


void i2c_write_byte(uint8_t address, uint8_t reg, uint8_t data) {
  uint8_t b[1] = {data};
  i2c_write(address, reg, b, 1);
}

void i2c_write_word(uint8_t address, uint8_t reg, uint16_t data) {
  uint8_t b[2] = {highByte(data), lowByte(data)};
  i2c_write(address, reg, b, 2);
}
