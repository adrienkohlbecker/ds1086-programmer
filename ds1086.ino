#include <Arduino.h>
#include <Wire.h>
#include <limits.h>

#include "helper.h"
#include "i2c.h"
#include "ds1086.h"

///// CHANGE THE VALUES BELOW ///////
  // Target frequency in Hertz
  uint32_t desired_f = 14000000;
  // Fine adjustment of the final frequency, use it to calibrate your chip by measuring the output clock with an oscilloscope.
  // This is added to the DAC value, so changes the frequency in increments of (10kHz / 2^prescaler), can be positive or negative.
  int fine_adjust = 138;
  // Dithering amount (DITHERING_2PCT or DITHERING_4PCT)
  dithering_t desired_dithering = DITHERING_2PCT;
///// DONE //////

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  Serial.begin(9600);

  Serial.print("connecting to address: ");
  printByteAsHex(ADDRESS);
  Serial.println();

  Serial.println();
  Serial.println("***CURRENT VALUES***");
  Serial.println();

  Serial.print("default offset: ");
  uint8_t default_offset = read_reg_range();
  printByteAsHex(default_offset);
  Serial.println();

  addr_t addr = read_reg_addr();
  Serial.print("write_contents: ");
  printByteAsBool(addr.write_contents);
  Serial.print("; device_address: ");
  printByteAsHex(addr.device_address);
  Serial.println();

  uint8_t current_offset = read_reg_offset();
  Serial.print("offset: ");
  Serial.print(current_offset);
  Serial.print(" delta: ");
  int8_t delta = (signed)(current_offset-default_offset);
  Serial.print(delta);
  Serial.print(" from ");
  Serial.print(offsetStart(delta));
  Serial.print(" to ");
  Serial.print(offsetEnd(delta));
  Serial.println();

  Serial.print("dac: ");
  Serial.print(read_reg_dac());
  Serial.println();

  prescaler_t prescaler = read_reg_prescaler();
  Serial.print("prescaler: ");
  Serial.print(prescaler.prescaler);
  Serial.print(" dithering: ");
  Serial.print(prescaler.dithering == DITHERING_4PCT ? "4%" : "2%");
  Serial.println();

  Serial.println();
  Serial.println("***NEW VALUES***");
  Serial.println();

  Serial.print("desired frequency: ");
  Serial.print(desired_f);
  Serial.println();

  // COMPUTE PRESCALER

  uint32_t master_f = 0;
  uint8_t desired_prescaler;

  for (desired_prescaler = 0; desired_prescaler<9; desired_prescaler++) {
    master_f = desired_f * (1 << desired_prescaler);
    if (master_f >= 66000000 && master_f <= 133000000) {
      break;
    }
  }

  if (master_f < 66000000 || master_f > 133000000) {
    Serial.println("Unable to find prescaler, master frequency is out of bounds.");
    return;
  }

  Serial.print("prescaler: ");
  Serial.print(1<<desired_prescaler);
  Serial.print(" target master frequency: ");
  Serial.print(master_f);
  Serial.println();

  // COMPUTE OFFSET

  bool found = false;
  uint32_t min_dist = ULONG_MAX;
  uint8_t desired_offset;

  for (int8_t i=-6; i<=6; i++) {
    uint32_t start = offsetStart(i);
    uint32_t end = offsetEnd(i);

    if ((master_f > end) || (master_f < start)) {
      continue;
    }

    found = true;

    uint32_t middle = (start + end)/2;
    uint32_t dist = (master_f>middle) ? (master_f-middle) : (middle-master_f);
    if (dist < min_dist) {
      min_dist = dist;
      desired_offset = i;
    }
  }

  if (!found) {
    Serial.println("Unable to find offset.");
    return;
  }

  Serial.print("offset: ");
  Serial.print(desired_offset);
  Serial.print(" from ");
  Serial.print(offsetStart(desired_offset));
  Serial.print(" to ");
  Serial.print(offsetEnd(desired_offset));
  Serial.println();

  // COMPUTE DAC

  uint32_t start = offsetStart(desired_offset);
  uint32_t desired_dac = (master_f - start) / 10000; // integer division
  uint32_t final_f = start+desired_dac*10000;

  uint32_t error = master_f - final_f;
  uint32_t otherError = (final_f+10000) - master_f; // do we get less error by rounding desired_dac up?
  if (otherError < error) {
    desired_dac += 1;
    final_f += 10000;
  }

  Serial.print("dac: ");
  Serial.print(desired_dac);
  Serial.print(" final master frequency: ");
  Serial.print(final_f);
  Serial.println();

  // FINE ADJUSTMENT

  desired_dac += fine_adjust;
  final_f += fine_adjust*10000;

  Serial.print("fine adjust: ");
  Serial.print(fine_adjust);
  Serial.print(" final adjusted output frequency: ");
  Serial.print(final_f/(1<<desired_prescaler));
  Serial.println();

  // WRITE VALUES

  prescaler_t p;
  p.dithering = desired_dithering;
  p.prescaler = desired_prescaler;

  write_reg_prescaler(p);
  delay(100);
  write_reg_offset(default_offset+desired_offset);
  delay(100);
  write_reg_dac(desired_dac);
  delay(100);
  write_eeprom();
  delay(100);

  Serial.println("done");
  Serial.println();
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    delay(1000);
}
