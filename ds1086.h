const uint8_t DEVICE_IDENTIFIER = 0b1011 << 3;
const uint8_t DEVICE_ADDRESS =     0b000 << 0;

const uint8_t ADDRESS = DEVICE_IDENTIFIER | DEVICE_ADDRESS;

const uint8_t REG_PRESCALER = 0x02;
const uint8_t REG_DAC       = 0x08;
const uint8_t REG_OFFSET    = 0x0E;
const uint8_t REG_ADDR      = 0x0D;
const uint8_t REG_RANGE     = 0x37;
const uint8_t REG_WRITE_EE  = 0x3F;

enum dithering_t { DITHERING_4PCT, DITHERING_2PCT };

typedef struct s_addr
{
  bool write_contents;
  uint8_t device_address;
} addr_t;

typedef struct s_prescaler
{
  dithering_t dithering;
  uint8_t prescaler;
} prescaler_t;

uint32_t offsetStart(int8_t offset) {
  switch (offset)
  {
      case -6:
        return 61440000;
      case -5:
        return 66560000;
      case -4:
        return 71680000;
      case -3:
        return 76800000;
      case -2:
        return 81920000;
      case -1:
        return 87040000;
      case 0:
        return 92160000;
      case 1:
        return 97280000;
      case 2:
        return 102400000;
      case 3:
        return 107520000;
      case 4:
        return 112640000;
      case 5:
        return 117760000;
      case 6:
        return 122880000;
      default:
        Serial.print("Error: Invalid offset ");
        Serial.print(offset);
        Serial.println(", expected -6 to 6");
        return -1;
  }
}

uint32_t offsetEnd(int8_t offset) {
  switch (offset)
  {
      case -6:
        return 71670000;
      case -5:
        return 76790000;
      case -4:
        return 81910000;
      case -3:
        return 87030000;
      case -2:
        return 92150000;
      case -1:
        return 97270000;
      case 0:
        return 102390000;
      case 1:
        return 107510000;
      case 2:
        return 112630000;
      case 3:
        return 117750000;
      case 4:
        return 122870000;
      case 5:
        return 127990000;
      case 6:
        return 133110000;
      default:
        Serial.print("Error: Invalid offset ");
        Serial.print(offset);
        Serial.println(", expected -6 to 6");
        return -1;
  }
}

uint8_t read_reg_range() {
  uint8_t res = i2c_read_byte(ADDRESS, REG_RANGE);
  return res & 0b00011111;
}

uint8_t read_reg_offset() {
  uint8_t res = i2c_read_byte(ADDRESS, REG_OFFSET);
  return res & 0b00011111;
}

void write_reg_offset(uint8_t offset) {
  if (offset >= 32) {
    Serial.print("Error: invalid offset in write_reg_offset: ");
    Serial.print(offset);
    Serial.println(", expected 0 to 31");
    return;
  }
  i2c_write_byte(ADDRESS, REG_OFFSET, offset);
}

uint16_t read_reg_dac() {
  uint16_t res = i2c_read_word(ADDRESS, REG_DAC);
  return res >> 6;
}

void write_reg_dac(uint16_t dac) {
  if (dac >= 1024) {
    Serial.print("Error: invalid dac in write_reg_dac: ");
    Serial.print(dac);
    Serial.println(", expected 0 to 1023");
    return;
  }
  i2c_write_word(ADDRESS, REG_DAC, dac << 6);
}

addr_t read_reg_addr() {
  uint8_t res = i2c_read_byte(ADDRESS, REG_ADDR);
  addr_t a;
  a.write_contents = res & 0b00001000;
  a.device_address = res & 0b00000111;
  return a;
}

prescaler_t read_reg_prescaler() {
  uint8_t res = i2c_read_byte(ADDRESS, REG_PRESCALER);
  prescaler_t p;
  if (res & 0b00010000) {
    p.dithering = DITHERING_2PCT;
  } else {
    p.dithering = DITHERING_4PCT;
  }
  p.prescaler = res & 0b00001111;
  return p;
}

void write_reg_prescaler(prescaler_t p) {
  if (p.prescaler >= 9) {
    Serial.print("Error: invalid prescaler in write_reg_prescaler: ");
    Serial.print(p.prescaler);
    Serial.println(", expected 0 to 8");
    return;
  }

  if (p.dithering != DITHERING_2PCT && p.dithering != DITHERING_4PCT) {
    Serial.print("Error: invalid dithering in write_reg_prescaler: ");
    Serial.print(p.dithering);
    Serial.println(", expected DITHERING_2PCT or DITHERING_4PCT");
    return;
  }

  uint8_t b;
  b |= p.dithering << 4;
  b |= p.prescaler << 0;

  i2c_write_byte(ADDRESS, REG_PRESCALER, b);
}

void write_eeprom() {
  i2c_write_void(ADDRESS, REG_DAC);
}
