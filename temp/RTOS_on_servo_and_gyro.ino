#include <Arduino_FreeRTOS.h>
// ! uncomlpete code

// + Ultrasound start  -------------------------------------
int critical_zone = 25;
int critical_zone_buzzer = 4;

int rotation_speed_delay = 40; // angle (++ or --) after (rotation_speed)ms
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
// + ultrasound end  ---------------------------------------

// ~ gyro start *************************
#include "Wire.h"                                          // This library allows you to communicate with I2C devices.
const int MPU_ADDR = 0x68;                                 // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
// int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
// int16_t temperature; // variables for temperature data
int mainX = 0, mainY = 0, mainZ = 0;
int softMargin = 150, global_X = 0, global_Y = 0, global_Z = 0;
char tmp_str[7]; // temporary variable used in convert function
char *convert_int16_to_str(int16_t i)
{ // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
    sprintf(tmp_str, "%6d", i);
    return tmp_str;
}
int gy_beep = 0;
// ~ gyro ends ***************************

// + servo start ------------------------
//---------------------------------------
#include <Servo.h>
Servo Myservo;
int pos;
bool ArraysInitialized = false;
bool warningLED = true;
int BuzzerBeeping = 0;
bool servo_Rotaion = true;
int d1[19];
int d2[19];
//---------------------------------------
// + servo end   ------------------------

void Gyro_manager(void *pvParameters);
void Servo_manager(void *pvParameters);
int pin1 = 5; // efective on Sender pin: 4 and reciver pin: D0 (1st) (Red)
int pin2 = 7; // efective on Sender pin: 3 and reciver pin: D1 (2nd) (Blue)
int pin3 = 3; // efective on Sender pin: 1 and reciver pin: D2 (3rd) (Green)
int pin4 = 8; // efective on Sender pin: 2 and reciver pin: D3 (4th) (Green mini)
// the setup function runs once when you press reset or power the board

void setup()
{
    Serial.begin(9600);
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    pinMode(pin4, OUTPUT);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, HIGH);
    Myservo.attach(2);
    pinMode(warning_zone_Led, OUTPUT);
    pinMode(critical_zone_buzzer, OUTPUT);
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT);  // Sets the echoPin as an INPUT

    pinMode(trigPin2, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(echoPin2, INPUT);  // Sets the echoPin as an INPUT
    Serial.println(
        "Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
    Serial.println("with Arduino UNO R3");
    Wire.begin();
    Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
    Wire.write(0x6B);                 // PWR_MGMT_1 register
    Wire.write(0);                    // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);

    // Now set up two tasks to run independently.
    xTaskCreate(
        Gyro_manager,
        "Gyro_manager" // A name just for humans
        ,
        128 // Stack size
        ,
        NULL, 2 // priority
        ,
        NULL);

    xTaskCreate(
        Servo_manager, "Servo_manager", 128 // This stack size can be checked & adjusted by reading Highwater
        ,
        NULL, 1 // priority
        ,
        NULL);

    // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
    // Empty. Things are done in Tasks.
}
int getPinNumber(int required_pin)
{
    if (required_pin == 1)
    {
        return pin1;
    }
    else if (required_pin == 2)
    {
        return pin2;
    }
    else if (required_pin == 3)
    {
        return pin3;
    }
    else if (required_pin == 4)
    {
        return pin4;
    }
    else
    {
        Serial.println("Invalid pin number! (getPinNumber error)");
        return 0;
    }
}
/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void Gyro_manager(void *pvParameters) // This is a task.
{
    (void)pvParameters;
    while (true)
    {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x3B);                        // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
        Wire.endTransmission(false);             // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
        Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers

        // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
        accelerometer_x = Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
        accelerometer_y = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
        accelerometer_z = Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
        Serial.print("---> aX = ");
        Serial.print(convert_int16_to_str(accelerometer_x));
        Serial.print(" | aY = ");
        Serial.print(convert_int16_to_str(accelerometer_y));
        Serial.print(" | aZ = ");
        Serial.print(convert_int16_to_str(accelerometer_z));
        Serial.println(" ");
        vTaskDelay(300 / portTICK_PERIOD_MS);
        mainX = accelerometer_x;
        mainY = accelerometer_y;
        mainZ = accelerometer_z;
        if (!(((global_X >= mainX - softMargin) && (global_X <= mainX + softMargin)) ||
              ((global_Y >= mainY - softMargin) && (global_Y <= mainY + softMargin)) ||
              ((global_Z >= mainZ - softMargin) && (global_Z <= mainZ + softMargin))))
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
            global_X = mainX;
            global_Y = mainY;
            global_Z = mainZ;
            if (gy_beep == 0)
            {
                gy_beep++;
            }
            else if (gy_beep == 1)
            {
                // custom_beep(2000, 200);
                // sendRFmsg(1);
                Serial.println("#######################");
                gy_beep--;
            }
        }
    }
}

void Servo_manager(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    // initialize serial communication at 9600 bits per second:
    // Serial.begin(9600);
    while (true)
    {
        if (servo_Rotaion)
        {
            // delay(300);
            vTaskDelay(300 / portTICK_PERIOD_MS);
        }
        for (pos = 0; pos <= 180; pos++)
        {
            // if (Serial.available() >= 1)
            // {
            //     choice = Serial.parseInt();
            //     if (choice >= 1)
            //     {
            //         inputHandler(choice);
            //     }
            // }

            Myservo.write(pos);
            // delay(rotation_speed_delay);
            vTaskDelay(rotation_speed_delay / portTICK_PERIOD_MS);

            if (pos % display_reading_after == 0)
            {
                // blynk(20);

                Serial.print("Angle : " + String(pos) + " -> ");

                update_distance(true);
            }
        }

        // delay(300);
        vTaskDelay(300 / portTICK_PERIOD_MS);

        for (pos = 180; pos >= 0; pos--)
        {
            // if (Serial.available() >= 1)
            // {
            //     choice = Serial.parseInt();
            //     if (choice >= 1)
            //     {
            //         inputHandler(choice);
            //     }
            // }

            Myservo.write(pos);
            // delay(rotation_speed_delay);
            vTaskDelay(rotation_speed_delay / portTICK_PERIOD_MS);

            if (pos % display_reading_after == 0)
            {
                // blynk(20);

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
    //....
    }
}
void update_distance(bool check)
{
    digitalWrite(trigPin, LOW);
    // delayMicroseconds(2);
    vTaskDelay(2 / portTICK_PERIOD_MS);
    digitalWrite(trigPin, HIGH);
    // delayMicroseconds(10);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    digitalWrite(trigPin2, LOW);
    // delayMicroseconds(2);
    vTaskDelay(2 / portTICK_PERIOD_MS);
    digitalWrite(trigPin2, HIGH);
    // delayMicroseconds(10);
    vTaskDelay(10 / portTICK_PERIOD_MS);
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