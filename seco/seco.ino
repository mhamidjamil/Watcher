//$ last work 02/Feb/27 [02:27 AM]
// # version 0.1 LED oop implemented

#define Buzzer 4
#define LED 6

#include "c_led.h"
c_led c_led;

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(Buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String receivedString = Serial.readStringUntil('\n');
    processString(receivedString);
  }
}

void processString(String commandString) {
  Serial.println("working on: " + commandString);
  if (isIn(commandString, "led on")) {
    c_led.led_on();
  } else if (isIn(commandString, "led off")) {
    c_led.led_off();
  } else if (isIn(commandString, "buzzer on")) {
    digitalWrite(Buzzer, HIGH); // Turn Buzzer on
  } else if (isIn(commandString, "buzzer off")) {
    digitalWrite(Buzzer, LOW); // Turn Buzzer off
  } else {
    Serial.println("Nothing executed");
  }
}

void led_on() { digitalWrite(LED, HIGH); }

void led_off() { digitalWrite(LED, LOW); }

void buzzer_on() { digitalWrite(Buzzer, HIGH); }

void buzzer_off() { digitalWrite(Buzzer, LOW); }

bool isIn(String mainString, String toFind) {
  return mainString.indexOf(toFind) != -1;
}
