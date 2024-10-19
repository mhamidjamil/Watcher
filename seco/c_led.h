// Cspiffs.h
#ifndef C_LED
#define C_LED_H
#include <Arduino.h>

#define LED 6

class c_led {

private:
  int led_pin = 6;

public:
  c_led();
  void led_on();
  void led_off();

  void blink(int times, int delay_);
  void print(String tempStr);
  void println(String tempStr);                 // actually Serail.println()
  void println(String tempStr, bool condition); // only print if it has to be
};

#endif
