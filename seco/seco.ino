//$ last work 02/Feb/27 [02:27 AM]
// # version 0.1 LED oop implemented

#define Buzzer 4
#define LED 13

#include "c_led.h"
c_led c_led;

void setup() {
  Serial.begin(115200); // Initialize serial communication
  pinMode(Buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String receivedString = Serial.readStringUntil('\n');
    // String receivedString = Serial.readString();
    processString(receivedString);
    println("receivedString: " + receivedString);
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
  } else if (isIn(commandString, "blink for")) {
    // blink for {5} delay: [1000]
    int times = 0, delay_ = 0;
    if (!updateValues(commandString, &times, &delay_))
      Serial.println("Any thing bad happen code: 39");
    c_led.blink(times, delay_);
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

void println(String tempStr) { Serial.println(tempStr); }

bool updateValues(String mainString, int *time, int *delay_) {
  // Find the position of the opening and closing curly braces
  int openCurlyBrace = mainString.indexOf('{');
  int closeCurlyBrace = mainString.indexOf('}');

  // Find the position of the opening and closing square brackets
  int openSquareBracket = mainString.indexOf('[');
  int closeSquareBracket = mainString.indexOf(']');

  // Check if any of the delimiters were not found
  if (openCurlyBrace == -1 || closeCurlyBrace == -1 ||
      openSquareBracket == -1 || closeSquareBracket == -1) {
    // Some delimiter is missing
    return false;
  }

  // Extract the data between curly braces and square brackets
  String valueInCurlyBraces =
      mainString.substring(openCurlyBrace + 1, closeCurlyBrace);
  String valueInSquareBrackets =
      mainString.substring(openSquareBracket + 1, closeSquareBracket);

  // Convert the extracted strings to integers
  *time = valueInCurlyBraces.toInt();
  *delay_ = valueInSquareBrackets.toInt();

  // Print the extracted values
  Serial.print("Value in curly braces: ");
  Serial.println(*time);
  Serial.print("Value in square brackets: ");
  Serial.println(*delay_);

  return true; // Indicate success
}
