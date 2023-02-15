void print(String input) { Serial.print(input); }
void println(String output) { Serial.println(output); }

void strManager(String input);
void setup() { Serial.begin(9600); }
void loop() {
  // take string as a input and send it to function strManager
  if (Serial.available())
    strManager(Serial.readStringUntil('\n'));
}
void strManager(String input) {
  println("working on : " + input);
  // break down string like 2.4.56 into 2, 4, 56 and print them after storing in
  // variables
  int a = input.substring(0, input.indexOf('.')).toInt();
  input = input.substring(input.indexOf('.') + 1);
  println("value : " + String(a));
  if (input.indexOf('.') != -1) {
    strManager(input);
  } else {
    int b = input.toInt();
    println("value : " + String(b));
  }
}