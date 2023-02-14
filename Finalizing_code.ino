// ! void loop need some time to adjest it self according to AlertStatus
// (distance) value with previous value.
//$ 12:29 AM 15/FEB/22
byte AlertStatus = 4;
//  Alert Status = 1: Alert when d1's value changes
//  Alert Status = 2: Alert when d2's value changes
//  Alert Status = 3: Alert when d1's or d2's value changes
//  Alert Status = 4: Alert when Gyro and d1's or d2's value changes
//  Alert Status = 5: Alert when Gyro value changes
//  Alert Status = 6: Alert when Gyro and d1's value changes
//  Alert Status = 7: Alert when Gyro and d2's value changes
// Alert Status = 8: No Alerts
// * ---------------------------------------------------------------------------------------------->    servo start   <------------
#include <Servo.h>
Servo Myservo;
int pos;
bool ArraysInitialized = false;
bool warningLED = true;
bool BuzzerBeeping = false;
bool servo_Rotaion = true;
#define display_reading_after 18 //  (180/display_reading_after) = x,(10)
#define array_size ((180 / display_reading_after) + 1)
int d1[array_size];
int d2[array_size];
int rotation_speed_delay = 50;  // angle (++ or --) after (rotation_speed)ms
byte softMargin_ultraSound = 2; // x inches changes will be negliected
// so increasing it will slow down rotation speed
void update_distance();
// * ----------------------------------------------------------------------------------------------->    servo ends   <------------
int choice = 0;
// int input_timeout = 10000;
// # Functions =============================================
int getPinNumber(int required_pin);
void switchManager(int PinNo_, int status);
void TestStream(int delay_);
void BinaryManager(int number);
void deciaml_to_binary(int number);
void SwitchInverter(int a, int b, int c, int d, bool status);
void TestStream(int delay_);
void BinaryManager(int number);
void sendRFmsg(int msgCode);
void check_gy_sensor(bool print_records);
void inputHandler(int choice);
void servoRotation();
void update_distance(bool check);
// void check_critical_distance();
// void check_warning_distance();
void beep();
void custom_beep(int beep_for, int delay_bt_beep);
void blynk(int defined_delay);
String getString();
void choise_handler(int *p);
bool change_Detector(int value_to_be_compare, int previous_value, int margin);
// # 20+ functions Defined =====================================

//  +---------------------------------------------------> Ultrasound start  <---
byte critical_zone = 25;
byte critical_zone_buzzer = 4;

byte warning_zone = 50;
byte warning_zone_Led = 6;

int alarm_time = 2000;
// int temp_alrm_time = alarm_time;

//  so after 10 degree readings will be printed
#define echoPin 12 //  attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 11 // attach pin D3 Arduino to pin Trig of HC-SR04

#define echoPin2 9  //  attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin2 10 // attach pin D3 Arduino to pin Trig of HC-SR04

long duration; // variable for the duration of sound wave travel
int distance;  // variable for the distance measurement

long duration2; // variable for the duration of sound wave travel
int distance2;  // variable for the distance measurement
//  +---------------------------------------------------------------------------------------->
//  +-------------------------------------------->  Ultrasound Ends <----------
int aggressive_monitoring = 0;
// `------------------------------------------->  RF sender config  <----------
#define pin1 5 // Sender pin: 4 and reciver pin: D0 (1st) (Red)
#define pin2 7 // Sender pin: 3 and reciver pin: D1 (2nd) (Blue)
#define pin3 3 // Sender pin: 1 and reciver pin: D2 (3rd) (Green)
#define pin4 8 // Sender pin: 2 and reciver pin: D3 (4th) (Green mini)
byte defaultDelay = 50;
int getPinNumber(int required_pin) {
  if (required_pin == 1) {
    return pin1;
  } else if (required_pin == 2) {
    return pin2;
  } else if (required_pin == 3) {
    return pin3;
  } else if (required_pin == 4) {
    return pin4;
  } else {
    Serial.println(F("Invalid pin number! (getPinNumber error)"));
    return 0;
  }
}
void offOuput() {
  delay(defaultDelay);
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, HIGH);
  delay(defaultDelay);
}
void deciaml_to_binary(int inpt) {
  // Serial.println("deciaml_to_binary : " + String(inpt));
  int out1 = inpt % 2;
  int out2 = 0;
  int out3 = 0;
  int out4 = 0;
  inpt = inpt / 2;
  if (inpt >= 2) {
    out2 = inpt % 2;
    inpt = inpt / 2;
    if (inpt >= 2) {
      out3 = inpt % 2;
      inpt = inpt / 2;
      if (inpt >= 2) {
        out4 = inpt % 2;
        inpt = inpt / 2;
      } else if (inpt == 1) {
        out4 = 1;
      }
    } else if (inpt == 1) {
      out3 = 1;
    }
  } else if (inpt == 1) {
    out2 = 1;
  }
  // Serial.println("Managed output: " + String(out4) + ", " + String(out3) + ",
  // " + String(out2) + ", " + String(out1));
  delay(defaultDelay);
  SwitchInverter(out4, out3, out2, out1, true);
  offOuput();
}
void SwitchInverter(int inpuT1, int inpuT2, int inpuT3, int inpuT4,
                    bool invert) {
  int out_1;
  int out_2;
  int out_3;
  int out_4;
  if (invert) {
    out_1 = !inpuT1;
    out_2 = !inpuT2;
    out_3 = !inpuT3;
    out_4 = !inpuT4;
  } else {
    out_1 = inpuT1;
    out_2 = inpuT2;
    out_3 = inpuT3;
    out_4 = inpuT4;
  }
  delay(defaultDelay);
  digitalWrite(pin1, out_1);
  digitalWrite(pin2, out_2);
  digitalWrite(pin3, out_3);
  digitalWrite(pin4, out_4);
  delay(defaultDelay);
  offOuput();
}
void switchManager(int pinNo, int invert_status) {

  if (invert_status == 0) {
    digitalWrite(getPinNumber(pinNo), HIGH);
  } else {
    digitalWrite(getPinNumber(pinNo), LOW);
  }
}
void TestStream(int delay_) {
  Serial.println("Delay time: " + String(delay_));
  for (int i = 1; i <= 4; i++) {
    switchManager(i, 1);
    delay(delay_);
    switchManager(i, 0);
    // delay(delay_);
  }
}
void BinaryManager(int number) {
  // Serial.println("Binary manager got : " + String(number));
  for (; number >= 15; number -= 15) {
    SwitchInverter(1, 1, 1, 1, true);
    delay(defaultDelay);
  }
  deciaml_to_binary(number);
}
void sendRFmsg(int msgCode) {
  // 1 for movment detected by GYRO
  // 2 for movment detected by ULTRASOUND
  // 3 for movment detected by both
  if (msgCode == 1) {
    BinaryManager(msgCode);
    Serial.println(F("Gyro detected movment, msg sent"));
  } else if (msgCode == 2) {
    BinaryManager(msgCode);
    Serial.println(F("ultra sound sensor (1) detected movment, msg sent"));
  } else if (msgCode == 3) {
    BinaryManager(msgCode);
    Serial.println(F("ultra sound sensor (2) detected movment, msg sent"));
  } else if (msgCode == 4) {
    BinaryManager(msgCode);
    Serial.println(F("Both ultra sound sensors detected movment, msg sent"));
  } else if (msgCode == 5) {
    BinaryManager(msgCode);
    Serial.println(F("Both detected movment, msg sent"));
  } else {
    Serial.println(F("msg code not defined"));
  }
}
// `---------------------------------------> RF sender config ends  <----------

//~---------------------------------------------->   gyro starts   <----------
int temp_send_if_servo_off = 0;
bool gyro_monitoring = true;
#include "Wire.h" // This library allows you to communicate with I2C devices.

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to
                           // HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y,
    accelerometer_z; // variables for accelerometer raw data
// int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
// int16_t temperature; // variables for temperature data
int mainX = 0, mainY = 0;
int softMargin = 350, global_X = 0, global_Y = 0, global_Z = 0;
char tmp_str[7]; // temporary variable used in convert function
char *convert_int16_to_str(
    int16_t i) { // converts int16 to string. Moreover, resulting strings will
                 // have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}
int gy_beep = 0;
void check_gy_sensor(bool print_records) {
  // if (servo_Rotaion)
  // {
  //     servo_Rotaion = false;
  // }
  delay(100);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(
      0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and
             // MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(
      false); // the parameter indicates that the Arduino will send a restart.
              // As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2,
                   true); // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in
  // the same variable
  accelerometer_x =
      Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H)
                                      // and 0x3C (ACCEL_XOUT_L)
  accelerometer_y =
      Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H)
                                      // and 0x3E (ACCEL_YOUT_L)
  accelerometer_z =
      Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H)
                                      // and 0x40 (ACCEL_ZOUT_L)
  if (print_records) {
    Serial.print(F("--->aX = "));
    Serial.print(convert_int16_to_str(accelerometer_x));
    Serial.print(F(" | aY = "));
    Serial.print(convert_int16_to_str(accelerometer_y));
    Serial.print(F(" | aZ = "));
    Serial.print(convert_int16_to_str(accelerometer_z));
    Serial.println(F(" "));
  }

  mainX = accelerometer_x;
  mainY = accelerometer_y;
  // mainZ = accelerometer_z;
  if (((change_Detector(mainX, global_X, softMargin)) ||
       (change_Detector(mainY, global_Y, softMargin)))) {
    if (!print_records) {
      Serial.print(F("!@ aX = "));
      Serial.print(global_X);
      Serial.print(F(" -> "));
      Serial.print(mainX);
      Serial.print(F(" | aY = "));
      Serial.print(global_Y);
      Serial.print(F(" -> "));
      Serial.print(mainY);
      // Serial.print(F(" | aZ = "));
      // Serial.print(convert_int16_to_str(accelerometer_z));
      Serial.println(F(" #"));
      global_X = mainX;
      global_Y = mainY;
      // global_Z = mainZ;
    }
    if (mainX != 0 || mainY != 0) {
      if (gy_beep == 0) {
        Serial.println(F("( @_ignored_@ )"));
        gy_beep++;
      } else if (gy_beep >= 1) {
        if (AlertStatus > 3 && AlertStatus != 8) {
          if (servo_Rotaion) {
            custom_beep(2000, 200);
          } else {
            beep();
          }
          sendRFmsg(1);
        }
        Serial.println(F("#######################"));
        gy_beep = 0;
      }
    }
  }
}
//~--------------------------------------------------->  gyro ends   <----------
// % ----------------------------------------------------------------------
void inputHandler(int choice) {
  Serial.println(F("input Handler call"));
  // choice = Serial.parseInt();
  // choice = getString().toInt();
  if (choice == 1) { // to set new values of variables
    Serial.println(F("Changing setting...."));
    Serial.println(F("Avaiable variable to change : "));
    Serial.println("1: critical_zone ," + String(critical_zone));
    Serial.println("2: warning_zone ," + String(warning_zone));
    Serial.println("3: alarm_time ," + String(alarm_time));
    // Serial.println("4: input_timeout ," + String(input_timeout));
    Serial.println("5: rotation_speed_delay ," + String(rotation_speed_delay));
    Serial.println("6: BuzzerBeeping ," + String(BuzzerBeeping));
    Serial.println("7: softMargin ," + String(softMargin));
    // Serial.println("6: display_reading_after ," +
    // String(display_reading_after));

    choice = getString().toInt();
    Serial.println("we got : " + String(choice));
    if (choice == 1) {
      // Serial.println("old value : " + String(critical_zone));
      choice = critical_zone;
      choise_handler(&choice);
      critical_zone = choice;
      // Serial.println("new value: " + String(critical_zone));
    } else if (choice == 2) {
      choice = warning_zone;
      choise_handler(&choice);
      warning_zone = choice;
    } else if (choice == 3) {
      choise_handler(&alarm_time);
    }
    // else if (choice == 4)
    // {
    //     choise_handler(&input_timeout);
    // }
    else if (choice == 5) {
      // choice = rotation_speed_delay;
      choise_handler(&rotation_speed_delay);
      // rotation_speed_delay = choice;
    } else if (choice == 6) {
      if (BuzzerBeeping) {
        choice = 1;
      } else {
        choice = 0;
      }
      choise_handler(&choice);
      if (choice == 1) {
        BuzzerBeeping = true;
      } else {
        BuzzerBeeping = false;
      }
      // BuzzerBeeping = tempvar_;
    } else if (choice == 7) {
      choise_handler(&softMargin);
    }
    // if (choice == 1) {
    //   Serial.println("Old value of critical_zone = " + critical_zone);
    //   Serial.println("Enter new value : ");
    //   choice = getint();
    // }
  } else if (choice == 2) { // to modify functionality
    Serial.println(F("Direct call..."));
    Serial.println(F("Enter 1 for LED  "));
    Serial.println(F("Enter 2 for Buzzer "));
    Serial.println(F("Enter 3 for Servo operations "));
    Serial.println(F("Enter 4 for Gyro operations "));
    Serial.println(F("Enter 5 for Alert status "));
    Serial.println(F("Enter 6 for aggressive monitoring mode"));
    choice = getString().toInt();
    if (choice == 1) { // LED work
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter 1 enable warning led blynk "));
      Serial.println(F("Enter 2 disable warning led blynk "));
      Serial.println(F("Enter 3 Turn LED on "));
      Serial.println(F("Enter 4 Turn LED off "));
      choice = getString().toInt();
      if (choice == 1) {
        warningLED = true;
      } else if (choice == 2) {
        warningLED = false;
      } else if (choice == 3) {
        digitalWrite(warning_zone_Led, HIGH);
      } else if (choice == 4) {
        digitalWrite(warning_zone_Led, LOW);
      }
    } else if (choice == 2) { // buzzer work
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter 1 to Turn Buzzer on "));
      Serial.println(F("Enter 2 to turn onbeeping  "));
      Serial.println(F("Enter 3 to force off Buzzer  "));
      choice = getString().toInt();
      if (choice == 1) {
        Serial.println(F("Enter delay time "));
        choice = getString().toInt();
        digitalWrite(critical_zone_buzzer, HIGH);
        delay(choice);
        digitalWrite(critical_zone_buzzer, LOW);
      } else if (choice == 2) {
        BuzzerBeeping = true;
      } else if (choice == 3) {
        BuzzerBeeping = false;
      }
    } else if (choice == 3) { // servo work
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter 1 to stop servo_Rotaion"));
      Serial.println(F("Enter 2 to Start servo_Rotaion"));
      Serial.println(F("Enter 3 to move servo to specific angle"));
      choice = getString().toInt();
      if (choice == 1) {
        servo_Rotaion = false;
        Serial.println(F("servo_Rotaion = false"));
        softMargin = 200;
      } else if (choice == 2) {
        servo_Rotaion = true;
        Serial.println(F("servo_Rotaion = true"));
        softMargin = 350;
      } else if (choice == 3) {
        Serial.println(F("------------------------------------------"));
        Serial.println(F("Enter angle to move servo to : "));
        choice = getString().toInt();
        Serial.println(F("For how long : "));
        int delay_temp = getString().toInt();
        Myservo.write(choice);
        delay(delay_temp);
      }
    } else if (choice == 4) {
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter 1 to stop gyro"));
      Serial.println(F("Enter 2 to Start gyro"));
      choice = getString().toInt();
      if (choice == 1) {
        gyro_monitoring = false;
        Serial.println(F("gyro = false"));
      } else if (choice == 2) {
        gyro_monitoring = true;
        Serial.println(F("gyro = true"));
      }
    } else if (choice == 5) { // Alert manager
      //  Alert Status = 1: Alert when d1's value changes
      //  Alert Status = 2: Alert when d2's value changes
      //  Alert Status = 3: Alert when d1's or d2's value changes
      //  Alert Status = 4: Alert when Gyro and d1's or d2's value changes
      //  Alert Status = 5: Alert when Gyro value changes
      //  Alert Status = 6: Alert when Gyro and d1's value changes
      //  Alert Status = 7: Alert when Gyro and d2's value changes
      // Alert Status = 8: No Alerts
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter New value : "));
      Serial.println(F("Alert when d1's value changes"));
      Serial.println(F("Alert when d2's value changes"));
      Serial.println(F("Alert when d1's or d2's value changes"));
      Serial.println(F("Alert when Gyro and d1's or d2's value changes"));
      Serial.println(F("Alert when Gyro value changes"));
      Serial.println(F("Alert when Gyro and d1's value changes"));
      Serial.println(F("Alert when Gyro and d2's value changes"));
      Serial.println(F("No Alerts"));

      AlertStatus = getString().toInt();
    } else if (choice == 6) { // Aggresive monitoring
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter 1 to stop aggressive monitoring"));
      Serial.println(F("Enter 2 to Start aggressive monitoring"));
      choice = getString().toInt();
      if (choice == 1) {
        AlertStatus = 4;
        softMargin = 350;
        BuzzerBeeping = false;
        servo_Rotaion = true;
        alarm_time = 3000;
        // Serial.println(F("aggressive_monitoring = false"));
      } else if (choice == 2) {
        Serial.println(F("------------------------------------------"));
        Serial.println(F("Enter 1 to active on D1 (stop)"));
        Serial.println(F("Enter 2 to active on D2 (stop)"));
        Serial.println(F("Enter 5 to active on just Gyro (Servo_stop)"));
        Serial.println(F("Enter 6 to active on D1 (Stop) & Gyro"));
        Serial.println(F("Enter 7 to active on D2 (stop) & Gyro"));
        AlertStatus = getString().toInt();
        if (!(AlertStatus == 1 || AlertStatus == 2 || AlertStatus == 5 ||
              AlertStatus == 6 || AlertStatus == 7)) {
          Serial.println(F("Invalid choice"));
        } else {
          softMargin = 200;
          BuzzerBeeping = true;
          servo_Rotaion = false;
          alarm_time = 10000; // 10 seconds
        }
        // Serial.println(F("aggressive_monitoring = true"));
      }
    }
  } else if (choice == 3) { // RF module
    Serial.println(F("------------------------------------------"));
    Serial.println(F("Enter 1 to Force pin high/low"));
    Serial.println(F("Enter 2 to send Binary data"));
    Serial.println(F("Enter 3 to test all output pins"));
    choice = getString().toInt();
    if (choice == 1) {
      int tempdaly;
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter pin number"));
      choice = getString().toInt();
      Serial.println(F("Enter Delay : "));
      tempdaly = getString().toInt();
      int choice2;
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter 1 to force pin high"));
      Serial.println(F("Enter 2 to force pin low"));
      choice2 = getString().toInt();
      if (choice2 == 1) {
        digitalWrite(getPinNumber(choice), LOW);
        delay(tempdaly);
        if (tempdaly != 420) {
          digitalWrite(getPinNumber(choice), HIGH);
        }
      } else if (choice2 == 2) {
        digitalWrite(getPinNumber(choice), HIGH);
      }
    } else if (choice == 2) {
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter decimal Number (<16) : "));
      choice = getString().toInt();

      Serial.println("Sending " + String(choice));
      BinaryManager(choice);
    } else if (choice == 3) {
      Serial.println(F("------------------------------------------"));
      Serial.println(F("Enter delay : "));
      choice = getString().toInt();
      TestStream(choice);
    }
  }
  choice = 0;
  Serial.println(F("Handler out"));
}
bool change_Detector(int value_to_be_compare, int previous_value, int margin) {
  if ((value_to_be_compare >= previous_value - margin) &&
      (value_to_be_compare <= previous_value + margin)) {
    return false;
  } else {
    return true;
  }
}
void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  Myservo.attach(2);
  pinMode(warning_zone_Led, OUTPUT);
  pinMode(critical_zone_buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);  // Sets the echoPin as an INPUT

  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin2, INPUT);  // Sets the echoPin as an INPUT
  Serial.begin(9600);
  Serial.println(F("Ultrasonic Sensor HC-SR04 Test"));
  Serial.println(F("with Arduino UNO R3"));
  Wire.begin();
  Wire.beginTransmission(
      MPU_ADDR);    // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}
void loop() {

  if (Serial.available() >= 1) {
    choice = Serial.parseInt();
    if (choice >= 1) {
      inputHandler(choice);
    }
  }
  // check_gy_sensor(true);
  if (AlertStatus != 8) {
    if (servo_Rotaion) {
      servoRotation();
      update_distance(false);
    } else if (AlertStatus != 5 && AlertStatus != 8) {
      update_distance();
    }
    if (!servo_Rotaion && (AlertStatus >= 4 && AlertStatus <= 7)) {
      temp_send_if_servo_off++;
      if (temp_send_if_servo_off % 100 == 0) {
        check_gy_sensor(true);
        temp_send_if_servo_off = 0;
      } else {
        check_gy_sensor(false);
      }
    }
  }
}
void servoRotation() {
  // Serial.println(F("ServoRotation called"));

  if (servo_Rotaion) {
    delay(100);
  }
  for (pos = 0; pos <= 180 && servo_Rotaion; pos++) {

    if (Serial.available() >= 1) {
      choice = Serial.parseInt();
      if (choice >= 1) {
        inputHandler(choice);
      }
    }
    Myservo.write(pos);
    delay(rotation_speed_delay);

    if (pos % display_reading_after == 0) {
      blynk(20);
      if (gyro_monitoring && ArraysInitialized) {
        check_gy_sensor(false);
      }
      // Serial.println(F("gyro out"));

      Serial.print("Angle : " + String(pos) + " -> ");

      update_distance(true);
      // Serial.println(F("update_distance out"));
    }
  }
  // Serial.println(F("ServoRotation 180 out"));
  delay(200);

  for (pos = 180; pos >= 0 && servo_Rotaion; pos--) {
    if (Serial.available() >= 1) {
      choice = Serial.parseInt();
      if (choice >= 1) {
        inputHandler(choice);
      }
    }

    Myservo.write(pos);
    delay(rotation_speed_delay);

    if (pos % display_reading_after == 0) {
      blynk(20);
      if (gyro_monitoring && ArraysInitialized) {
        check_gy_sensor(false);
      }

      Serial.print("Angle : " + String(pos) + " -> ");

      update_distance(true);
    }
  }
  if (!ArraysInitialized) {
    ArraysInitialized = true;
    Serial.println(F("data in array is"));

    int ijk = 0;
    Serial.print("D1 : ");
    for (; ijk < sizeof(d1) / sizeof(int); ijk++) {
      Serial.print(String(d1[ijk]) + ",");
    }
    Serial.println("");

    ijk = 0;
    Serial.print("D2 : ");
    for (; ijk < sizeof(d1) / sizeof(int); ijk++) {
      Serial.print(String(d2[ijk]) + ",");
    }
    Serial.println("");
  }
}
void update_distance(bool check) {
  update_distance();
  Serial.print(F("D1 : "));
  Serial.print(distance / 2.54);
  Serial.print(F(", D2 : "));
  Serial.print(distance2 / 2.54);
  Serial.println(F(" in"));

  if (!ArraysInitialized && check && servo_Rotaion) { // initializing arrays
    // Serial.print("Angle : " + String(pos));
    // Serial.println(", index : " + String(pos / display_reading_after));
    d1[pos / display_reading_after] = (distance / 2.54);
    d2[pos / display_reading_after] = (distance2 / 2.54);
  } else if (ArraysInitialized && check && servo_Rotaion &&
             (AlertStatus != 8 && AlertStatus != 5)) {
    if ((change_Detector((distance / 2.54), (d1[pos / display_reading_after]),
                         softMargin_ultraSound)) ||
        (change_Detector((distance2 / 2.54), (d2[pos / display_reading_after]),
                         softMargin_ultraSound))) {
      int msg_code = 0;
      // Serial.println(F("ultra sound change detected"));
      if ((change_Detector((distance / 2.54), (d1[pos / display_reading_after]),
                           softMargin_ultraSound)) &&
          (AlertStatus != 2 || AlertStatus != 7 || AlertStatus != 8)) {
        // Serial.println(F("ultra sound change detected on D1"));
        if (((distance / 2.54) < 450) &&
            (d1[pos / display_reading_after]) < 450) {
          Serial.print("#->(" + String(pos) + ")->");
          Serial.print("previous value : " +
                       String(d1[pos / display_reading_after]));
          Serial.println(" current value : " + String(distance / 2.54));
          d1[pos / display_reading_after] = (distance / 2.54);
          msg_code += 5;
        } else {
          Serial.print(F("@(_ignored_) D1 value changes ( "));
          Serial.println(String(d1[pos / display_reading_after]) + " --> " +
                         String(distance / 2.54) + " )");
          d1[pos / display_reading_after] = (distance / 2.54);
        }
      }
      if ((change_Detector((distance2 / 2.54),
                           (d2[pos / display_reading_after]),
                           softMargin_ultraSound)) &&
          (AlertStatus != 1 || AlertStatus != 6 || AlertStatus != 8)) {
        // Serial.println(F("ultra sound change detected on D2"));
        if (((distance2 / 2.54) < 450) &&
            (d2[pos / display_reading_after]) < 450) {
          Serial.print("##->(" + String(pos) + ")->");
          Serial.print("previous value : " +
                       String(d2[pos / display_reading_after]));
          Serial.println(" current value : " + String(distance2 / 2.54));
          d2[pos / display_reading_after] = (distance2 / 2.54);
          msg_code += 10;
        } else {
          Serial.print(F("@(_ignored_) D2 value changes ("));
          Serial.println(String(d2[pos / display_reading_after]) + " --> " +
                         String(distance2 / 2.54) + " )");
          d2[pos / display_reading_after] = (distance2 / 2.54);
        }
      }
      if (msg_code == 5) {
        sendRFmsg(2);
      } else if (msg_code == 10) {
        sendRFmsg(3);
      } else if (msg_code == 15) {
        sendRFmsg(4);
      }
    }
  }
}
void beep() {
  //   int temp_alrm_time = beep_for;
  digitalWrite(warning_zone_Led, HIGH);
  for (; alarm_time > 0;
       alarm_time -= 400) { // decrement should be 100 (50(HIGH)+50(LOW))
    // but i use 400 to finish it earlier
    if (BuzzerBeeping) {
      digitalWrite(critical_zone_buzzer, HIGH);
      delay(50);
    } else {
      digitalWrite(warning_zone_Led, LOW);
      delay(50);
      digitalWrite(warning_zone_Led, HIGH);
      delay(50);
      alarm_time -= 50;
    }
    digitalWrite(critical_zone_buzzer, LOW);
    delay(50);
  }
  digitalWrite(warning_zone_Led, LOW);
}
void custom_beep(int beep_for, int delay_bt_beep) {
  digitalWrite(warning_zone_Led, HIGH);
  for (; beep_for > 0;
       beep_for -=
       (delay_bt_beep * 2)) { // decrement should be 100 (50(HIGH)+50(LOW))
    // but i use 400 to finish it earlier
    if (BuzzerBeeping) {
      digitalWrite(critical_zone_buzzer, HIGH);
      delay(50);
    } else {
      digitalWrite(warning_zone_Led, LOW);
      delay(50);
      digitalWrite(warning_zone_Led, HIGH);
      delay(50);
      alarm_time -= 50;
    }
    digitalWrite(critical_zone_buzzer, LOW);
    delay(50);
  }
  digitalWrite(warning_zone_Led, LOW);
}
void blynk(int defined_delay) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(defined_delay); // wait for a second
  digitalWrite(LED_BUILTIN,
               LOW); // turn the LED off by making the voltage LOW
  // delay(1000);
  if (warningLED) {
    digitalWrite(warning_zone_Led, HIGH);
    delay(defined_delay);
    digitalWrite(warning_zone_Led, LOW);
  }
}
String getString() {
  String sdata = "";
  char ch = '0';
  bool condit = true;
  while (condit) {

    // if (Serial.available() > 0)
    // {
    ch = Serial.read(); // get the character
    delay(20);
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || (ch == '=')) {
      sdata += ch;
    } else if (ch == '!') {
      Serial.println(F("Bypassed"));
      loop();
    } else if (ch == '.') {
      // Serial.print("Sr we got ");
      // Serial.println(sdata);
      condit = false;
      delay(100);
      // FileNameLoop = sdata;
    } else if (ch == ',') {
      Serial.println(F("Str cleared\n"));
      sdata = "";
      // Print(sdata);
      // condit = false;
      // FileNameLoop = sdata;
    }
    // }
  }
  return sdata;
}
void choise_handler(int *p) {
  Serial.print(F("Enter new value : "));
  int newvalue = getString().toInt();
  Serial.println(newvalue);
  Serial.print("Value Changed : " + String(*p) + "-->");
  *p = newvalue;
  Serial.println(String(*p));
  // Serial.println("new value : " + String(*p));
}
void update_distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2;
}