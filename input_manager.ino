String String_holder = "";
void print(String input) { Serial.print(input); }
void print(int input) { Serial.print(input); }
void println(String output) { Serial.println(output); }

void holder_manager(String input);
void setup() { Serial.begin(9600); }
void loop() {
  // take string as a input and send it to function holder_manager
  if (Serial.available())
    String_holder = Serial.readStringUntil('\n');
  // holder_manager(Serial.readStringUntil('\n'));
  while (String_holder != "") {
    print(holder_manager());
    print(" _ ");
  }
}
int holder_manager() {
  println("working on : " + String_holder);
  // break down string like 2.4.56 into 2, 4, 56 and print them after storing in
  // variables
  //   println("value : " + String(a));
  if (String_holder.indexOf('.') != -1) {
    int a = String_holder.substring(0, String_holder.indexOf('.')).toInt();
    String_holder = String_holder.substring(String_holder.indexOf('.') + 1);
    // holder_manager(input);
    return a;
  } else {
    int b = String_holder.toInt();
    String_holder = "";
    return b;
    // println("value : " + String(b));
  }
}
