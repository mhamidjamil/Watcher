// servo start
#include <Servo.h>
Servo Myservo;
int pos;
bool ArraysInitialized = false;
bool warningLED = true;
int BuzzerBeeping = 0;
bool servo_Rotaion = true;
int d1[19];
int d2[19];
// servo end
int choice = 0;
int delayed = 10;
int input_timeout = 10000;
// Ultrasound start
int critical_zone = 25;
int critical_zone_buzzer = 4;

int rotation_speed_delay = 30; // angle (++ or --) after (rotation_speed)ms
// so increasing it will slow down rotation speed

int warning_zone = 50;
int warning_zone_Led = 6;

int alarm_time = 2000;
int temp_alrm_time = alarm_time;
int display_reading_after = 10; //  (180/display_reading_after) = x,(18)
                                //  so after 10 degree readings will be printed
#define echoPin 12              //  attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 11              // attach pin D3 Arduino to pin Trig of HC-SR04

#define echoPin2 9  //  attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin2 10 // attach pin D3 Arduino to pin Trig of HC-SR04

long duration; // variable for the duration of sound wave travel
int distance;  // variable for the distance measurement

long duration2; // variable for the duration of sound wave travel
int distance2;  // variable for the distance measurement
// ultrasound end

// gy start ********************************

#include "Wire.h" // This library allows you to communicate with I2C devices.

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
// int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
// int16_t temperature; // variables for temperature data
int mainX = 0, mainY = 0, mainZ = 0;
int softMargin = 65, global_X = 0, global_Y = 0, global_Z = 0;
int led_counter = 0;
char tmp_str[7]; // temporary variable used in convert function

char *convert_int16_to_str(int16_t i)
{ // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
    sprintf(tmp_str, "%6d", i);
    return tmp_str;
}
int gy_beep = 0;
// gy ends ***************************
void check_gy_sensor()
{
    // if (servo_Rotaion)
    // {
    //     servo_Rotaion = false;
    // }
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);                        // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
    Wire.endTransmission(false);             // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
    Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers

    // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
    accelerometer_x = Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    accelerometer_y = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    accelerometer_z = Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    Serial.print("aX = ");
    Serial.print(convert_int16_to_str(accelerometer_x));
    Serial.print(" | aY = ");
    Serial.print(convert_int16_to_str(accelerometer_y));
    Serial.print(" | aZ = ");
    Serial.print(convert_int16_to_str(accelerometer_z));
    Serial.print(" ");

    mainX = accelerometer_x;
    mainY = accelerometer_y;
    mainZ = accelerometer_z;
    if (!(((global_X >= mainX - softMargin) && (global_X <= mainX + softMargin)) ||
          ((global_Y >= mainY - softMargin) && (global_Y <= mainY + softMargin)) ||
          ((global_Z >= mainZ - softMargin) && (global_Z <= mainZ + softMargin))))
    {
        global_X = mainX;
        global_Y = mainY;
        global_Z = mainZ;
        if (gy_beep == 0)
        {
            gy_beep++;
        }
        else if (gy_beep == 1)
        {
            custom_beep(3000, 100);
            Serial.println("#######################");
            gy_beep--;
        }
    }
}
void inputHandler(int choice)
{
    Serial.println("input Handler call");
    // choice = Serial.parseInt();
    // choice = getString().toInt();
    if (choice == 1)
    { // to set new values of variables
        Serial.println("Changing setting....");
        Serial.println("Avaiable variable to change : ");
        Serial.println("1: critical_zone ," + String(critical_zone));
        Serial.println("2: warning_zone ," + String(warning_zone));
        Serial.println("3: alarm_time ," + String(alarm_time));
        Serial.println("4: input_timeout ," + String(input_timeout));
        Serial.println("5: rotation_speed_delay ," + String(rotation_speed_delay));
        Serial.println("6: BuzzerBeeping ," + String(BuzzerBeeping));
        Serial.println("7: softMargin ," + String(softMargin));
        // Serial.println("6: display_reading_after ," +
        // String(display_reading_after));

        choice = getString().toInt();
        Serial.println("we got : " + String(choice));
        if (choice == 1)
        {
            // Serial.println("old value : " + String(critical_zone));
            choise_handler(&critical_zone);
            // Serial.println("new value: " + String(critical_zone));
        }
        else if (choice == 2)
        {
            choise_handler(&warning_zone);
        }
        else if (choice == 3)
        {
            choise_handler(&alarm_time);
        }
        else if (choice == 4)
        {
            choise_handler(&input_timeout);
        }
        else if (choice == 5)
        {
            choise_handler(&rotation_speed_delay);
        }
        else if (choice == 6)
        {
            choise_handler(&BuzzerBeeping);
        }
        else if (choice == 7)
        {
            choise_handler(&softMargin);
        }
        // if (choice == 1) {
        //   Serial.println("Old value of critical_zone = " + critical_zone);
        //   Serial.println("Enter new value : ");
        //   choice = getint();
        // }
    }
    else if (choice == 2)
    {
        Serial.println("Direct call...");
        Serial.println("Enter 1 for LED  ");
        Serial.println("Enter 2 for Buzzer ");
        Serial.println("Enter 3 for Servo operations ");
        choice = getString().toInt();
        if (choice == 1)
        {
            Serial.println("Enter 1 enable warning led blynk ");
            Serial.println("Enter 2 disable warning led blynk ");
            Serial.println("Enter 3 Turn LED on ");
            Serial.println("Enter 4 Turn LED off ");
            choice = getString().toInt();
            if (choice == 1)
            {
                warningLED = true;
            }
            else if (choice == 2)
            {
                warningLED = false;
            }
            else if (choice == 3)
            {
                digitalWrite(warning_zone_Led, HIGH);
            }
            else if (choice == 4)
            {
                digitalWrite(warning_zone_Led, LOW);
            }
        }
        else if (choice == 2)
        {
            Serial.println("Enter 1 to Turn Buzzer on ");
            Serial.println("Enter 2 to turn onbeeping  ");
            Serial.println("Enter 3 to force off Buzzer  ");
            choice = getString().toInt();
            if (choice == 1)
            {
                Serial.println("Enter delay time ");
                choice = getString().toInt();
                digitalWrite(critical_zone_buzzer, HIGH);
                delay(choice);
                digitalWrite(critical_zone_buzzer, LOW);
            }
            else if (choice == 2)
            {
                BuzzerBeeping = true;
            }
            else if (choice == 3)
            {
                BuzzerBeeping = false;
            }
        }
        else if (choice == 3)
        {
            Serial.println("Enter 1 to stop servo_Rotaion");
            Serial.println("Enter 2 to Start servo_Rotaion");
            choice = getString().toInt();
            if (choice == 1)
            {
                servo_Rotaion = false;
                Serial.println("servo_Rotaion = false");
            }
            else if (choice == 2)
            {
                servo_Rotaion = true;
                Serial.println("servo_Rotaion = true");
            }
        }
    }
    else if (choice == 3)
    {
        // RF module
        Serial.println("Enter 1 to Force pin high");
        Serial.println("Enter 2 to send Binary data");
        Serial.println("Enter 3 to test all output pins");
        choice = getString().toInt();
        if (choice == 1)
        {
            int tempdaly;
            Serial.println("Enter pin number");
            choice = getString().toInt();
            Serial.println("Enter Delay:");
            tempdaly = getString().toInt();
            if (choice > 4 || choice < 1)
            {
                Serial.println("Invalid pin number");
            }
            else
            {
                digitalWrite(choice, HIGH);
                delay(tempdaly);
                digitalWrite(choice, LOW);
            }
        }
        else if (choice == 2)
        {
            Serial.println("Enter decimal Number (<16) : ");
            choice = getString().toInt();

            Serial.println("Sending " + String(choice));
            BinaryManager(choice);
        }
        else if (choice == 3)
        {
            Serial.println("Enter pin number");
            choice = getString().toInt();
            for (int i = 0; i < choice; i++)
            {
                digitalWrite(i, LOW);
                delay(100);
                digitalWrite(i, HIGH);
            }
        }
    }
    choice = 0;
    Serial.println("Handler out");
}
// * RF sender config :
int pin1 = 5; // efective on Sender pin: 4 and reciver pin: D0 (1st) (Red)
int pin2 = 7; // efective on Sender pin: 3 and reciver pin: D1 (2nd) (Blue)
int pin3 = 3; // efective on Sender pin: 1 and reciver pin: D2 (3rd) (Green)
int pin4 = 8; // efective on Sender pin: 2 and reciver pin: D3 (4th) (Green mini)
int defaultDelay = 48;
void offOuput()
{
    delay(defaultDelay);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, HIGH);
    delay(defaultDelay);
}
void deciaml_to_binary(int inpt)
{
    Serial.println("deciaml_to_binary : " + String(inpt));
    int out1 = inpt % 2;
    int out2 = 0;
    int out3 = 0;
    int out4 = 0;
    inpt = inpt / 2;
    if (inpt >= 2)
    {
        out2 = inpt % 2;
        inpt = inpt / 2;
        if (inpt >= 2)
        {
            out3 = inpt % 2;
            inpt = inpt / 2;
            if (inpt >= 2)
            {
                out4 = inpt % 2;
                inpt = inpt / 2;
            }
            else if (inpt == 1)
            {
                out4 = 1;
            }
        }
        else if (inpt == 1)
        {
            out3 = 1;
        }
    }
    else if (inpt == 1)
    {
        out2 = 1;
    }
    Serial.println("Managed output: " + String(out4) + ", " + String(out3) + ", " + String(out2) + ", " + String(out1));
    delay(defaultDelay);
    SwitchInverter(out4, out3, out2, out1, true);
    offOuput();
}
void SwitchInverter(int inpuT1, int inpuT2, int inpuT3, int inpuT4, bool invert)
{
    int out_1;
    int out_2;
    int out_3;
    int out_4;
    if (invert)
    {
        out_1 = !inpuT1;
        out_2 = !inpuT2;
        out_3 = !inpuT3;
        out_4 = !inpuT4;
    }
    else
    {
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
void switchManager(int pinNo, int status)
{
    int PinNo_;
    switch (pinNo)
    {
    case 1:
        PinNo_ = pin1;
        break;
    case 2:
        PinNo_ = pin2;
        break;
    case 3:
        PinNo_ = pin3;
        break;
    case 4:
        PinNo_ = pin4;
        break;

    default:
        Serial.println("Invalid pin number");
        break;
    }

    if (status == 1)
    {
        digitalWrite(PinNo_, HIGH);
    }
    else
    {
        digitalWrite(PinNo_, LOW);
    }
}
void TestStream(int delay_)
{
    Serial.println("Delay time: " + String(delay_));
    for (int i = 1; i <= 4; i++)
    {
        switchManager(i, 0);
        delay(delay_);
        switchManager(i, 1);
        delay(delay_);
    }
}
void BinaryManager(int number)
{
    Serial.println("Binary manager got : " + String(number));
    for (; number >= 15; number -= 15)
    {
        SwitchInverter(1, 1, 1, 1, true);
        delay(defaultDelay);
    }
    deciaml_to_binary(number);
}

bool manuallight = false;
// * RF reciver config end
void setup()
{
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
    Serial.begin(
        9600); // // Serial Communication is starting with 9600 of baudrate speed
    Serial.println(
        "Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
    Serial.println("with Arduino UNO R3");
    Wire.begin();
    Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
    Wire.write(0x6B);                 // PWR_MGMT_1 register
    Wire.write(0);                    // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
}
void loop()
{
    if (Serial.available() >= 1)
    {
        choice = Serial.parseInt();
        if (choice >= 1)
        {
            inputHandler(choice);
        }
    }
    check_gy_sensor();
    if (servo_Rotaion)
    {
        servoRotation();
    }
    // Clears the trigPin condition
    update_distance(false);

    //  check_warning_distance();
    //  check_critical_distance();2
    temp_alrm_time = alarm_time;
}
void servoRotation()
{

    if (servo_Rotaion)
    {
        delay(300);
    }
    for (pos = 0; pos <= 180; pos++)
    {
        if (Serial.available() >= 1)
        {
            choice = Serial.parseInt();
            if (choice >= 1)
            {
                inputHandler(choice);
            }
        }

        Myservo.write(pos);
        delay(rotation_speed_delay);

        if (pos % display_reading_after == 0)
        {
            blynk(20);

            Serial.print("Angle : " + String(pos) + " -> ");

            update_distance(true);
        }
    }

    delay(300);

    for (pos = 180; pos >= 0; pos--)
    {
        if (Serial.available() >= 1)
        {
            choice = Serial.parseInt();
            if (choice >= 1)
            {
                inputHandler(choice);
            }
        }

        Myservo.write(pos);
        delay(rotation_speed_delay);

        if (pos % display_reading_after == 0)
        {
            blynk(20);

            Serial.print("Angle : " + String(pos) + " -> ");

            update_distance(true);
        }
    }
    if (!ArraysInitialized)
    {
        ArraysInitialized = true;
        Serial.println("data in array is");

        int ijk = 0;
        Serial.print("D1 : ");
        for (; ijk < 18; ijk++)
        {
            Serial.print(String(d1[ijk]) + ",");
        }
        Serial.println("");

        ijk = 0;
        Serial.print("D2 : ");
        for (; ijk < 18; ijk++)
        {
            Serial.print(String(d2[ijk]) + ",");
        }
        Serial.println("");
    }
}
void update_distance(bool check)
{
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

    Serial.print("D1 : ");
    Serial.print(distance / 2.54);
    Serial.print(", D2 : ");
    Serial.print(distance2 / 2.54);
    Serial.println(" in");

    if (!ArraysInitialized && check && servo_Rotaion)
    { // initializing arrays
        // Serial.print("Angle : " + String(pos));
        // Serial.println(", index : " + String(pos / display_reading_after));
        d1[pos / display_reading_after] = (distance / 2.54);
        d2[pos / display_reading_after] = (distance2 / 2.54);
    }
}
void check_critical_distance()
{
    update_distance(false);
    if ((distance / 2.54) < critical_zone)
    {
        Serial.print("Critical distance");
        beep();
    }
}
void check_warning_distance()
{
    update_distance(false);
    if ((distance / 2.54) < warning_zone)
    {
        Serial.print("warning distance");
        // int temp_alrm_time = warn_for;
        for (; temp_alrm_time > 0; temp_alrm_time -= 200)
        {
            digitalWrite(warning_zone_Led, HIGH);
            delay(100);
            check_critical_distance();
            digitalWrite(warning_zone_Led, LOW);
            delay(100);
            check_critical_distance();
        }
    }
}
void beep()
{
    //   int temp_alrm_time = beep_for;
    digitalWrite(warning_zone_Led, HIGH);
    for (; temp_alrm_time > 0;
         temp_alrm_time -= 400)
    { // decrement should be 100 (50(HIGH)+50(LOW))
        // but i use 400 to finish it earlier
        if (BuzzerBeeping)
        {
            digitalWrite(critical_zone_buzzer, HIGH);
            delay(50);
        }
        else
        {
            digitalWrite(warning_zone_Led, LOW);
            delay(50);
            digitalWrite(warning_zone_Led, HIGH);
            delay(50);
            temp_alrm_time -= 50;
        }
        digitalWrite(critical_zone_buzzer, LOW);
        delay(50);
    }
    digitalWrite(warning_zone_Led, LOW);
}
void custom_beep(int beep_for, int delay_bt_beep)
{
    digitalWrite(warning_zone_Led, HIGH);
    for (; beep_for > 0; beep_for -= (delay_bt_beep * 2))
    { // decrement should be 100 (50(HIGH)+50(LOW))
        // but i use 400 to finish it earlier
        if (BuzzerBeeping)
        {
            digitalWrite(critical_zone_buzzer, HIGH);
            delay(50);
        }
        else
        {
            digitalWrite(warning_zone_Led, LOW);
            delay(50);
            digitalWrite(warning_zone_Led, HIGH);
            delay(50);
            temp_alrm_time -= 50;
        }
        digitalWrite(critical_zone_buzzer, LOW);
        delay(50);
    }
    digitalWrite(warning_zone_Led, LOW);
}
void blynk(int defined_delay)
{
    digitalWrite(LED_BUILTIN,
                 HIGH);             // turn the LED on (HIGH is the voltage level)
    delay(defined_delay);           // wait for a second
    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    // delay(1000);
    if (warningLED)
    {
        digitalWrite(warning_zone_Led, HIGH);
        delay(defined_delay);
        digitalWrite(warning_zone_Led, LOW);
    }
}
String getString()
{
    String sdata = "";
    char ch = '0';
    bool condit = true;
    while (condit)
    {

        // if (Serial.available() > 0)
        // {
        ch = Serial.read(); // get the character
        delay(20);
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || (ch == '='))
        {
            sdata += ch;
        }
        else if (ch == '!')
        {
            Serial.println("Bypassed");
            loop();
        }
        else if (ch == '.')
        {
            // Serial.print("Sr we got ");
            // Serial.println(sdata);
            condit = false;
            delay(100);
            // FileNameLoop = sdata;
        }
        else if (ch == ',')
        {
            Serial.println("Str cleared\n");
            sdata = "";
            // Print(sdata);
            // condit = false;
            // FileNameLoop = sdata;
        }
        // }
    }
    return sdata;
}
void choise_handler(int *p)
{
    Serial.print("Enter new value : ");
    int newvalue = getString().toInt();
    Serial.println(newvalue);
    Serial.print("Value Changed : " + String(*p) + "-->");
    *p = newvalue;
    Serial.println(String(*p));
    // Serial.println("new value : " + String(*p));
}