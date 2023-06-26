# ds1086-programmer

This arduino code is used to program Maxim's DS1086 programmable clock generator.

## Instructions

1. Connect 5V and GND from your Arduino to the DS1086 on pins 3 and 4 respectively.
1. Connect SDA and SCL from your Arduino to the DS1086 on pins 7 and 8 respectively
1. Edit `ds1086.ino` with your desired values:

```c
  // Target frequency in Hertz
  uint32_t desired_f = 14000000;
  // Fine adjustment of the final frequency, use it to calibrate your chip by measuring the output clock with an oscilloscope.
  // This is added to the DAC value, so changes the frequency in increments of (10kHz / 2^prescaler), can be positive or negative.
  int fine_adjust = 138;
  // Dithering amount (DITHERING_2PCT or DITHERING_4PCT)
  dithering_t desired_dithering = DITHERING_2PCT;
```

1. Upload the code to your Arduino. Your DS1086 is immediately programmed and you can see the built-in LED blinking.
1. In case you need it, debugging information about how the algorithm computes the necessary values for your desired clock frequency is available on the Arduino's serial port.
