#include <Arduino_FreeRTOS.h>

//  define two tasks for Blink & AnalogRead

void outputManager(void *pvParameters);
void LED_blink(void *pvParameters);
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
    // Now set up two tasks to run independently.
    xTaskCreate(
        outputManager,
        "Blink" // A name just for humans
        ,
        128 // Stack size
        ,
        NULL, 2 // priority
        ,
        NULL);

    xTaskCreate(
        LED_blink, "AnalogRead", 128 // This stack size can be checked & adjusted by reading Highwater
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

void outputManager(void *pvParameters) // This is a task.
{
    (void)pvParameters;
    while (true)
    {
        for (int i = 1; i <= 2; i++) // A Task shall never return or exit.
        {

            digitalWrite(getPinNumber(i), LOW);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            digitalWrite(getPinNumber(i), HIGH);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            // digitalWrite(13, HIGH);                // turn the LED on (HIGH is the voltage level)
            // vTaskDelay(1000 / portTICK_PERIOD_MS); // wait for one second
            // digitalWrite(13, LOW);                 // turn the LED off by making the voltage LOW
            // vTaskDelay(1000 / portTICK_PERIOD_MS); // wait for one second
        }
    }
}

void LED_blink(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    // initialize serial communication at 9600 bits per second:
    // Serial.begin(9600);
    while (true)
    {
        if (Serial.available() > 0)
        {
            int input = Serial.parseInt();
            // if (input > 0)
            if (input > 0)
            {
                if (input < 20)
                {
                    Serial.print("working on : " + String(input));
                    if (input >= 10)
                    {
                        digitalWrite((getPinNumber(input % 10)), HIGH);
                        // switchManager((input % 10), 0);
                        Serial.print(" - " + String((input % 10)) + " off");
                    }
                    else
                    {
                        digitalWrite(getPinNumber(input), LOW);
                        // switchManager(input, 1);
                        Serial.print(" - " + String(input) + " on");
                    }
                }
                // BinaryManager(input);
            }
            Serial.println();
        }
    }
}
