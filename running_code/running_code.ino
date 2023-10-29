//$ last work 29/October/23 [06:15 PM]
// @Version 2
// # Before changing serial functionality
//  * ---------------------------------------------------------------------------------------------->    servo start   <------------
//! D2 (Ultra Sound (2) is not function)
#include <Servo.h>
Servo Myservo;
int pos;
bool array_initialized = false;
bool warningLED = true;
bool buzzer_allowed = false;
bool servo_allowed = true;
#define display_reading_after 18 //  (180/display_reading_after) = x,(10)
#define array_size ((180 / display_reading_after) + 1)
int d1[array_size];
int d2[array_size];
int rotation_speed_delay = 50; // angle (++ or --) after (rotation_speed)ms
byte negligible_distance = 3;  // x inches changes will be neglected
// so increasing it will slow down rotation speed
void update_distance();
byte monitor_on = 0;
// * ----------------------------------------------------------------------------------------------->    servo ends   <------------
int choice = 0;
String string_holder = "2.5.2.3"; // monitor over gyro sensor
int aggressive_monitoring = 0;
// # Functions =============================================
int getPinNumber(int required_pin);
void switchManager(int PinNo_, int status);
void BinaryManager(int number);
void decimalToBinary(int number);
void SwitchInverter(int a, int b, int c, int d, bool status);
void testStream(int delay_);
void sendRFmsg(int msgCode);
void check_gy_sensor(bool print_records);
bool inputHandler(int choice);
void inputHandler(String str_input);
void servoRotation();
void update_distance(bool check);
// void check_critical_distance();
// void check_warning_distance();
void beep();
void custom_beep(int beep_for, int delay_bt_beep);
void blink(int defined_delay);
String getString();
void choiceHandler(int *p);
void choiceHandler(byte *p);
bool changeDetector(int value_to_be_compare, int previous_value, int margin);
int changeDetector(int val1, int val2);
int holderManager();
void LED_ON();
void LED_OFF();
void Buzzer_ON();
void Buzzer_OFF();
// # 20+ functions Defined =====================================

//  +---------------------------------------------------> Ultrasound start  <---
#define Buzzer 4
#define LED 6

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
//  +-------------------------------------------->  Ultrasound Ends <----------

// `------------------------------------------->  RF sender config  <----------
#define pin1 5 // Sender pin: 4 and receiver pin: D0 (1st) (Red)
#define pin2 7 // Sender pin: 3 and receiver pin: D1 (2nd) (Blue)
#define pin3 3 // Sender pin: 1 and receiver pin: D2 (3rd) (Green)
#define pin4 8 // Sender pin: 2 and receiver pin: D3 (4th) (Green mini)
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

void decimalToBinary(int input_) {
  // Serial.println("decimalToBinary : " + String(input_));
  int out1 = input_ % 2;
  int out2 = 0;
  int out3 = 0;
  int out4 = 0;
  input_ = input_ / 2;
  if (input_ >= 2) {
    out2 = input_ % 2;
    input_ = input_ / 2;
    if (input_ >= 2) {
      out3 = input_ % 2;
      input_ = input_ / 2;
      if (input_ >= 2) {
        out4 = input_ % 2;
        input_ = input_ / 2;
      } else if (input_ == 1) {
        out4 = 1;
      }
    } else if (input_ == 1) {
      out3 = 1;
    }
  } else if (input_ == 1) {
    out2 = 1;
  } else {
    if (input_ == 0) {
      Serial.println("input becomes 0");
    } else {
      Serial.println("unexpected error #135");
    }
  }
  // Serial.println("Managed output: " + String(out4) + ", " + String(out3) +
  // "," + String(out2) + ", " + String(out1));
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

void testStream(int delay_) {
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
  decimalToBinary(number);
}

void sendRFmsg(int msgCode) {
  // 1 for movement detected by GYRO
  // 2 for movement detected by ULTRASOUND
  // 3 for movement detected by both
  if (msgCode == 1) {
    BinaryManager(msgCode);
    Serial.println(F("Gyro detected movement, msg sent"));
  } else if (msgCode == 2) {
    BinaryManager(msgCode);
    Serial.println(F("ultra sound sensor (1) detected movement, msg sent"));
  } else if (msgCode == 3) {
    BinaryManager(msgCode);
    Serial.println(F("ultra sound sensor (2) detected movement, msg sent"));
  } else if (msgCode == 4) {
    BinaryManager(msgCode);
    Serial.println(F("Both ultra sound sensors detected movement, msg sent"));
  } else if (msgCode == 5) {
    BinaryManager(msgCode);
    Serial.println(F("Both detected movement, msg sent"));
  } else if (msgCode == 6) {
    BinaryManager(msgCode);
    Serial.println(F("Ultrasound 1 detected movement, msg sent"));
  } else if (msgCode == 7) {
    BinaryManager(msgCode);
    Serial.println(F("Ultrasound 2 detected movement, msg sent"));
  } else {
    Serial.println(F("msg code not defined"));
  }
}
// `---------------------------------------> RF sender config ends <----------

//~---------------------------------------------->   gyro starts   <----------
int temp_send_if_servo_off = 0;
bool gyro_monitoring = true;
#include "Wire.h" // This library allows you to communicate with I2C devices.

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set
                           // to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y,
    accelerometer_z; // variables for accelerometer raw data
// int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
// int16_t temperature; // variables for temperature data
int mainX = 0, mainY = 0;
byte negligible_motion_servo_on = 230;
byte negligible_motion_Servo_off = 130;

// , global_X = 0, global_Y = 0, global_Z = 0;
char tmp_str[7]; // temporary variable used in convert function
char *convert_int16_to_str(
    int16_t i) { // converts int16 to string. Moreover, resulting strings will
                 // have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}
byte gy_beep = 0;

void check_gy_sensor(bool print_records, int neg_motion) {
  delay(100);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and
  // MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false);
  // the parameter indicates that the Arduino will send a restart.
  // As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true);
  // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored
  // in the same variable
  accelerometer_x = Wire.read() << 8 | Wire.read();
  // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read() << 8 | Wire.read();
  // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read() << 8 | Wire.read();
  // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  if (print_records) {
    Serial.print(F("--->aX = "));
    Serial.print(convert_int16_to_str(accelerometer_x));
    Serial.print(F(" | aY = "));
    Serial.print(convert_int16_to_str(accelerometer_y));
    Serial.print(F(" | aZ = "));
    Serial.print(convert_int16_to_str(accelerometer_z));
    Serial.println(F(" "));
  }

  // mainZ = accelerometer_z;
  // if (((changeDetector(mainX, global_X, neg_motion)) ||
  //      (changeDetector(mainY, global_Y, neg_motion))))
  if (neg_motion != -1) {
    if (((changeDetector(accelerometer_x, mainX) +
          changeDetector(accelerometer_y, mainY)) /
         2) > neg_motion) {
      if (!print_records) {
        Serial.print(F("!@ aX = "));
        Serial.print(mainX);
        Serial.print(F(" -> "));
        Serial.print(accelerometer_x);
        Serial.print(F(" | aY = "));
        Serial.print(mainY);
        Serial.print(F(" -> "));
        Serial.print(accelerometer_y);
        // Serial.print(F(" | aZ = "));
        // Serial.print(convert_int16_to_str(accelerometer_z));
        Serial.print(" avg change (" +
                     String(((changeDetector(accelerometer_x, mainX) +
                              changeDetector(accelerometer_y, mainY)) /
                             2)) +
                     ") ");
        // global_X = mainX;
        // global_Y = mainY;
        // global_Z = mainZ;
      }
      if (mainX != 0 || mainY != 0) {
        if (gy_beep == 0) {
          Serial.println(F(" ( @_ignored_@ )"));
          gy_beep++;
        } else if (gy_beep >= 1) {
          custom_beep(alarm_time, 200);
          sendRFmsg(1);
          Serial.println(F(" Alert Send! "));
          gy_beep = 0;
        }
      }
    }
  }
  mainX = accelerometer_x;
  mainY = accelerometer_y;
}
//~--------------------------------------------------->  gyro ends <----------
// % -------------------------------------------------------------------------
bool inputHandler(int choice) {
  Serial.println(F("input Handler call"));
  // choice = Serial.parseInt();
  // if (choice > 10) {
  //   choice = getString().toInt();
  // }
  if (choice == 1) { // to set new values of variables
    Serial.println(F("Changing setting...."));
    Serial.println(F("Available variable to change : "));
    Serial.println("1: alarm_time ," + String(alarm_time));
    // Serial.println("2: input_timeout ," + String(input_timeout));
    Serial.println("3: rotation_speed_delay ," + String(rotation_speed_delay));
    Serial.println("4: negligible_motion_servo_on , " +
                   String(negligible_motion_servo_on));
    Serial.println("5: negligible_motion_Servo_off , " +
                   String(negligible_motion_Servo_off));
    Serial.println("6: negligible_distance , " + String(negligible_distance));
    // Serial.println("6: display_reading_after ," +
    // String(display_reading_after));

    choice = getString().toInt();
    // Serial.println("we got : " + String(choice));
    if (choice == 1) {
      choiceHandler(&alarm_time);
    }
    // else if (choice == 2)
    // {
    //     choiceHandler(&input_timeout);
    // }
    else if (choice == 3) {
      // choice = rotation_speed_delay;
      choiceHandler(&rotation_speed_delay);
      // rotation_speed_delay = choice;
    } else if (choice == 4) {
      choiceHandler(&negligible_motion_servo_on);
    } else if (choice == 5) {
      choiceHandler(&negligible_motion_Servo_off);
    } else if (choice == 6) {
      choiceHandler(&negligible_distance);
    } else {
      Serial.println("Invalid choice");
      return false;
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
    Serial.println(F("Enter 5 for aggressive monitoring mode"));
    choice = getString().toInt();
    // Serial.println("input -> ( " + String(choice) + " )");
    // + pointer for debugging
    if (choice == 1) { // LED work
      if (!warningLED) {
        Serial.println(F("Enter 1 to enable warning led blink "));
      } else if (warningLED) {
        Serial.println(F("Enter 2 to disable warning led blink "));
      }
      Serial.println(F("Enter 3 to Turn LED on (force)"));
      Serial.println(F("Enter 4 to Turn LED off "));
      choice = getString().toInt();
      // Serial.println("input -> ( " + String(choice) + " )");
      if (choice == 1) {
        warningLED = true;
      } else if (choice == 2) {
        warningLED = false;
      } else if (choice == 3) {
        digitalWrite(LED, HIGH);
      } else if (choice == 4) {
        digitalWrite(LED, LOW);
      } else {
        Serial.println(F("Invalid choice"));
        return false;
      }
    } else if (choice == 2) { // buzzer work
      if (buzzer_allowed) {
        Serial.println(F("Enter 1 to disable buzzer_allowed "));
      } else if (!buzzer_allowed) {
        Serial.println(F("Enter 2 to enable buzzer_allowed "));
      }
      Serial.println(F("Enter 3 to test buzzer (force on)"));
      choice = getString().toInt();
      // Serial.println("input -> ( " + String(choice) + " )");
      if (choice == 1) {
        buzzer_allowed = false;
      } else if (choice == 2) {
        buzzer_allowed = true;
      } else if (choice == 3) {
        Serial.print(F("Enter delay time : "));
        choice = getString().toInt();
        Serial.println(choice);
        digitalWrite(Buzzer, HIGH);
        delay(choice);
        digitalWrite(Buzzer, LOW);
      } else {
        Serial.println(F("Invalid choice"));
        return false;
      }
    } else if (choice == 3) { // servo work
      if (servo_allowed) {
        Serial.println(F("Enter 1 to stop servo_allowed"));
      } else if (!servo_allowed) {
        Serial.println(F("Enter 2 to start servo_allowed"));
      }
      Serial.println(F("Enter 3 to move servo to specific angle"));
      choice = getString().toInt();
      // Serial.println("input -> ( " + String(choice) + " )");
      if (choice == 1) {
        servo_allowed = false;
        Serial.println(F("servo_allowed = false"));
      } else if (choice == 2) {
        servo_allowed = true;
        Serial.println(F("servo_allowed = true"));
      } else if (choice == 3) {
        Serial.print(F("Enter angle to move servo to : "));
        choice = getString().toInt();
        Serial.println(choice);
        Serial.println(F("For how long : "));
        choice = getString().toInt();
        Serial.println(choice);
        Myservo.write(choice);
        delay(choice);
      } else {
        Serial.println(F("Invalid choice"));
        return false;
      }
    } else if (choice == 4) {
      if (gyro_monitoring) {
        Serial.println(F("Enter 1 to stop gyro"));
      } else if (!gyro_monitoring) {
        Serial.println(F("Enter 2 to Start gyro"));
      }
      Serial.println(F("Enter 3 to Force test gyro working"));
      choice = getString().toInt();
      // Serial.println("input -> ( " + String(choice) + " )");
      if (choice == 1) {
        gyro_monitoring = false;
        Serial.println(F("gyro = false"));
      } else if (choice == 2) {
        gyro_monitoring = true;
        Serial.println(F("gyro = true"));
      } else if (choice == 3) {
        Serial.print(F("For how long you want to test it? (in seconds) : "));
        choice = getString().toInt();
        Serial.println(choice);
        while (choice > 0) {
          check_gy_sensor(true, -1);
          delay(100);
          choice -= 100;
        }
      } else {
        Serial.println(F("Invalid choice"));
        return false;
      }
    } else if (choice == 5) { // Aggressive monitoring
      Serial.println(F("Enter 1 to stop aggressive monitoring"));
      Serial.println(F("Enter 2 to Start aggressive monitoring"));
      choice = getString().toInt();
      // Serial.println("input -> ( " + String(choice) + " )");
      if (choice == 1) {
        servo_allowed = true;
        buzzer_allowed = false;
        gyro_monitoring = true;
        alarm_time /= 2;
        // Serial.println(F("aggressive_monitoring = false"));
      } else if (choice == 2) {
        Serial.println(F("Enter 1 to active on D1 (stop) & gyro off"));
        Serial.println(F("Enter 2 to active on D2 (stop) & gyro off"));
        Serial.println(F("Enter 3 to active on just Gyro (Servo_stop)"));
        Serial.println(F("Enter 4 to active on D1 (Stop) & Gyro"));
        Serial.println(F("Enter 5 to active on D2 (stop) & Gyro"));
        int temp_6 = getString().toInt();
        // Serial.println("input -> ( " + String(temp_6) + " )");
        if (temp_6 == 1 || temp_6 == 2) {
          gyro_monitoring = false;
        } else if (temp_6 == 3 || temp_6 == 4 || temp_6 == 5) {
          gyro_monitoring = true;
        }
        if (temp_6 == 1 || temp_6 == 4) {
          monitor_on = 1;
        } else if (temp_6 == 2 || temp_6 == 5) {
          monitor_on = 2;
        } else if (temp_6 == 3) {
          monitor_on = 0;
        }
        //~ some variables have to change there values w.r.t situation
        servo_allowed = false;
        buzzer_allowed = true;
        warningLED = true;
        if (alarm_time < 5000) {
          alarm_time = alarm_time * 2;
        }
        // Serial.println(F("aggressive_monitoring = true"));
      }
    } else {
      Serial.println(F("Invalid choice"));
      return false;
    }
  } else if (choice == 3) { // RF module
    Serial.println(F("Enter 1 to Force pin high/low"));
    Serial.println(F("Enter 2 to send Binary data"));
    Serial.println(F("Enter 3 to test all output pins"));
    choice = getString().toInt();
    // Serial.println("input -> ( " + String(choice) + " )");
    if (choice == 1) {
      int temp_delay;
      Serial.print(F("Enter pin number : "));
      choice = getString().toInt();
      Serial.println(choice);
      Serial.println(F("Enter Delay : "));
      temp_delay = getString().toInt();
      Serial.println(temp_delay);
      int choice2;
      Serial.println(F("Enter 1 to force pin high"));
      Serial.println(F("Enter 2 to force pin low"));
      choice2 = getString().toInt();
      // Serial.println("input -> ( " + String(choice) + " )");
      if (choice2 == 1) {
        digitalWrite(getPinNumber(choice), LOW);
        delay(temp_delay);
        if (temp_delay != 420) {
          digitalWrite(getPinNumber(choice), HIGH);
        }
      } else if (choice2 == 2) {
        digitalWrite(getPinNumber(choice), HIGH);
      }
    } else if (choice == 2) {
      Serial.print(F("Enter decimal Number (<16) : "));
      choice = getString().toInt();
      Serial.println(choice);
      BinaryManager(choice);
    } else if (choice == 3) {
      Serial.print(F("Enter delay : "));
      choice = getString().toInt();
      Serial.println(choice);
      testStream(choice);
    } else {
      Serial.println(F("Invalid choice"));
      return false;
    }
  } else {
    Serial.println(F("Invalid choice"));
    return false;
  }
  choice = 0;
  Serial.println(F("Handler out"));
  return true;
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
  pinMode(LED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);  // Sets the echoPin as an INPUT

  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin2, INPUT);  // Sets the echoPin as an INPUT
  Serial.begin(115200);
  Serial.println(F("Activating Watcher..."));
  Wire.begin();
  Wire.beginTransmission(
      MPU_ADDR);    // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() {

  if (Serial.available() >= 1) {
    String tempStr = Serial.readStringUntil('\n');
    if (tempStr.length() > 0) {
      inputHandler(tempStr);
    }
  }
  if (servo_allowed) {
    servoRotation();
    //   update_distance(false);
  } else {
    {
      update_distance();
      if (monitor_on == 1 || monitor_on == 10) {
        d1[array_size - 1] = distance;

        if (changeDetector(d1[array_size - 1], d1[array_size - 2],
                           negligible_distance)) {
          if (monitor_on == 1) {

            Serial.println("@ ignored D1 changed (" +
                           String(d1[array_size - 2]) + " -> " +
                           String(d1[array_size - 1]) + ")");
            d1[array_size - 2] = d1[array_size - 1];
            // ! alert
            monitor_on = 10;
            delay(50);
          } else {
            Serial.println("D1 changed (" + String(d1[array_size - 2]) +
                           " -> " + String(d1[array_size - 1]) + ")");
            d1[array_size - 2] = d1[array_size - 1];
            sendRFmsg(6);
            beep();
            monitor_on = 1;
            delay(50);
          }
        }
      }

      if (monitor_on == 2 || monitor_on == 20) {
        d2[array_size - 1] = distance2;
        if (changeDetector(d2[array_size - 1], d2[array_size - 2],
                           negligible_distance)) {
          if (monitor_on == 2) {

            Serial.println("@ ignored D2 changed (" +
                           String(d2[array_size - 2]) + " -> " +
                           String(d2[array_size - 1]) + ")");
            d2[array_size - 2] = d2[array_size - 1];
            // ! alert
            monitor_on = 20;
            delay(50);
          } else {
            Serial.println("D2 changed (" + String(d2[array_size - 2]) +
                           " -> " + String(d2[array_size - 1]) + ")");
            d2[array_size - 2] = d2[array_size - 1];
            sendRFmsg(7);
            beep();
            monitor_on = 2;
            delay(50);
          }
        }
      }
    }
    if (gyro_monitoring) {
      check_gy_sensor(false, negligible_motion_Servo_off);
    }
  }
  if (negligible_motion_Servo_off == 130) {
    inputHandler(string_holder);
    negligible_motion_Servo_off -= 5;
  }
}

void servoRotation() {
  // Serial.println(F("ServoRotation called"));

  if (servo_allowed) {
    delay(100);
  }
  for (pos = 0; pos <= 180 && servo_allowed; pos++) {

    if (Serial.available() >= 1) {
      String tempStr = Serial.readStringUntil('\n');
      if (tempStr.length() > 0) {
        inputHandler(tempStr);
      }
    }
    Myservo.write(pos);
    delay(rotation_speed_delay);

    if (pos % display_reading_after == 0) {
      blink(20);
      if (gyro_monitoring && array_initialized) {
        check_gy_sensor(false, negligible_motion_servo_on);
      }
      // Serial.println(F("gyro out"));

      //   Serial.print("Angle : " + String(pos) + " -> ");

      update_distance(true);
      // Serial.println(F("update_distance out"));
    }
  }
  // Serial.println(F("ServoRotation 180 out"));
  delay(200);

  for (pos = 180; pos >= 0 && servo_allowed; pos--) {
    if (Serial.available() >= 1) {
      String tempStr = Serial.readStringUntil('\n');
      if (tempStr.length() > 0) {
        inputHandler(tempStr);
      }
    }

    Myservo.write(pos);
    delay(rotation_speed_delay);

    if (pos % display_reading_after == 0) {
      blink(20);
      if (gyro_monitoring && array_initialized) {
        check_gy_sensor(false, negligible_motion_servo_on);
      }

      //   Serial.print("Angle : " + String(pos) + " -> ");

      update_distance(true);
    }
  }
  if (!array_initialized) {
    array_initialized = true;
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
  if (check) {
    Serial.print("Angle : " + String(pos) + " -> ");
  }
  Serial.print(F("D1 : "));
  Serial.print(distance);
  Serial.print(F(", D2 : "));
  Serial.print(distance2);
  Serial.println(F(" in"));

  if (!array_initialized && check && servo_allowed) { // initializing arrays
    // Serial.print("Angle : " + String(pos));
    // Serial.println(", index : " + String(pos / display_reading_after));
    d1[pos / display_reading_after] = distance;
    d2[pos / display_reading_after] = distance2;
  } else if (array_initialized && check && servo_allowed) {
    if ((changeDetector(distance, (d1[pos / display_reading_after]),
                        negligible_distance)) ||
        (changeDetector(distance2, (d2[pos / display_reading_after]),
                        negligible_distance))) {
      int msg_code = 0;
      // Serial.println(F("ultra sound change detected"));
      if ((changeDetector(distance, (d1[pos / display_reading_after]),
                          negligible_distance))) {
        // Serial.println(F("ultra sound change detected on D1"));
        if ((distance < 450) && (d1[pos / display_reading_after]) < 450) {
          Serial.print("#->(" + String(pos) + ")->");
          Serial.print("previous value : " +
                       String(d1[pos / display_reading_after]));
          Serial.println(" current value : " + String(distance));
          d1[pos / display_reading_after] = distance;
          msg_code += 5;
        } else {
          Serial.print(F("@(_ignored_) D1 value changes ( "));
          Serial.println(String(d1[pos / display_reading_after]) + " --> " +
                         String(distance) + " )");
          d1[pos / display_reading_after] = distance;
        }
      }
      if ((changeDetector(distance2, (d2[pos / display_reading_after]),
                          negligible_distance))) {
        // Serial.println(F("ultra sound change detected on D2"));
        if ((distance2 < 450) && (d2[pos / display_reading_after]) < 450) {
          Serial.print("##->(" + String(pos) + ")->");
          Serial.print("previous value : " +
                       String(d2[pos / display_reading_after]));
          Serial.println(" current value : " + String(distance2));
          d2[pos / display_reading_after] = distance2;
          msg_code += 10;
        } else {
          Serial.print(F("@(_ignored_) D2 value changes ("));
          Serial.println(String(d2[pos / display_reading_after]) + " --> " +
                         String(distance2) + " )");
          d2[pos / display_reading_after] = distance2;
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
  LED_ON();
  int temp_time = alarm_time;
  // Serial.println("delay in beep : " + String(temp_time) + " ms");
  for (; temp_time > 0;
       temp_time -= 250) { // decrement should be 100 (50(HIGH)+50(LOW))
                           // but i use 400 to finish it earlier
    Buzzer_ON();
    delay(100);
    Buzzer_OFF();
    delay(50);
    LED_ON();
    delay(50);
    LED_OFF();
    delay(50);
  }
  LED_OFF();
}

void custom_beep(int beep_for, int delay_bt_beep) {
  LED_ON();
  for (; beep_for > 0;
       beep_for -=
       (delay_bt_beep * 2)) { // decrement should be 100 (50(HIGH)+50(LOW))
    // but i use 400 to finish it earlier
    if (buzzer_allowed) {
      Buzzer_ON();
      delay(50);
    } else {
      LED_OFF();
      delay(50);
      LED_ON();
      delay(50);
      alarm_time -= 50;
    }
    Buzzer_OFF();
    delay(50);
  }
  LED_OFF();
}

void blink(int defined_delay) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(defined_delay); // wait for a second
  digitalWrite(LED_BUILTIN,
               LOW); // turn the LED off by making the voltage LOW
                     // delay(1000);
  LED_ON();
  delay(defined_delay);
  LED_OFF();
}

String getString() {
  if (string_holder != "" && string_holder != " " && string_holder != ".") {
    return String(holderManager());
  } else {
    string_holder = "";
  }
  String sdata = "";
  char ch = '0';
  bool condition_ = true;
  while (condition_) {

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
      condition_ = false;
      delay(100);
      // FileNameLoop = sdata;
    } else if (ch == ',') {
      Serial.println(F("Str cleared\n"));
      sdata = "";
      // Print(sdata);
      // condition_ = false;
      // FileNameLoop = sdata;
    }
    // }
  }
  Serial.println();
  Serial.println(F("------------------------------------------"));
  return sdata;
}

void choiceHandler(int *p) {
  Serial.print(F("Enter new value : "));
  int newvalue = getString().toInt();
  Serial.println(newvalue);
  Serial.print("Value Changed : " + String(*p) + "-->");
  *p = newvalue;
  Serial.println(String(*p));
  // Serial.println("new value : " + String(*p));
}

void choiceHandler(byte *p) {
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
  distance = distance / 2.54;

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2;
  distance2 = distance2 / 2.54;
}

bool changeDetector(int value_to_be_compare, int previous_value, int margin) {
  if ((value_to_be_compare >= previous_value - margin) &&
      (value_to_be_compare <= previous_value + margin)) {
    return false;
  } else {
    return true;
  }
}

int changeDetector(int val1, int val2) {
  // int value3 = val1 - val2;
  // normalizing values for better results
  if (val1 < 0) {
    val1 = -val1;
  }
  if (val2 < 0) {
    val2 = -val2;
  }
  if ((val1 - val2) < 0) {
    return ((val1 - val2) * -1);
  } else {
    return (val1 - val2);
  }
}

int holderManager() {
  // println("working on : " + string_holder);
  // break down string like 2.4.56 into 2, 4, 56 and print them after storing in
  // variables
  //   println("value : " + String(a));
  if (string_holder.indexOf('!') != -1 || string_holder == " ") {
    string_holder = "";
    loop();
  }
  if (string_holder.indexOf('.') != -1) {
    int a = string_holder.substring(0, string_holder.indexOf('.')).toInt();
    string_holder = string_holder.substring(string_holder.indexOf('.') + 1);
    // holderManager(input);
    return a;
  } else {
    int b = string_holder.toInt();
    string_holder = "";
    return b;
    // println("value : " + String(b));
  }
}

void inputHandler(String str_input) {
  if (str_input.indexOf('!') != -1) {
    string_holder = "";
    loop();
  } else if (str_input == " ") {
    string_holder = "";
    loop();
  } else if (str_input.indexOf(' ') != -1) {
    string_holder = "";
    loop();
  } else if (str_input.indexOf('.') != -1) {
    string_holder = str_input;
    inputHandler(holderManager());
  } else {
    inputHandler(str_input.toInt());
  }
}

void LED_ON() {
  if (warningLED) {
    digitalWrite(LED, HIGH);
  }
}

void LED_OFF() { digitalWrite(LED, LOW); }

void Buzzer_ON() {
  if (buzzer_allowed)
    digitalWrite(Buzzer, HIGH);
}

void Buzzer_OFF() { digitalWrite(Buzzer, LOW); }
