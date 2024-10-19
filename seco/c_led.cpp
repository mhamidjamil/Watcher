#include "c_led.h"

c_led::c_led() {
  Serial.begin(115200);
  println("\t\t\t###___Constructor___###");
}

void c_led::led_on() {
  // to on led
  println("custom led on");
  digitalWrite(LED, HIGH);
}

void c_led::led_off() {
  // to on led
  println("custom led off");
  digitalWrite(LED, LOW);
}

void c_led::blink(int times, int delay_) {
  for (int i = 0; i < times; i++) {
    c_led::led_on();
    delay(delay_);
    c_led::led_off();
    if (delay_ < 500)
      delay(delay_);
    else
      delay(500);
  }
}

void c_led::println(String tempStr) { Serial.println(tempStr); }

void c_led::println(String tempStr, bool condition) {
  condition ? println(tempStr) : void();
}

void c_led::print(String tempStr) { Serial.print(tempStr); }
