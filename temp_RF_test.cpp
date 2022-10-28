// int pin1 = 3; // efective on Sender pin: 1 and reciver pin: D2 (3rd) (Green)
// int pin2 = 8; // efective on Sender pin: 2 and reciver pin: D3 (4th) (Green mini)
// int pin3 = 7; // efective on Sender pin: 3 and reciver pin: D1 (2nd) (Blue)
// int pin4 = 5; // efective on Sender pin: 4 and reciver pin: D0 (1st) (Red)

int pin1 = 5; // efective on Sender pin: 4 and reciver pin: D0 (1st) (Red)
int pin2 = 7; // efective on Sender pin: 3 and reciver pin: D1 (2nd) (Blue)
int pin3 = 3; // efective on Sender pin: 1 and reciver pin: D2 (3rd) (Green)
int pin4 = 8; // efective on Sender pin: 2 and reciver pin: D3 (4th) (Green mini)
int defaultDelay = 48;
void offOuput();
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
}
void offOuput()
{
    delay(defaultDelay);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, HIGH);
    delay(defaultDelay);
}
void SecondManager(int inpt)
{
    Serial.println("SecondManager : " + String(inpt));
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
    SecondManager(number);
}
int cmd = 0;
bool manuallight = false;
void loop()
{
    if (Serial.available() > 0)
    {
        int input = Serial.parseInt();
        // if (input > 0)
        if (input > 0)
        {
            if (input == 420)
            {
                manuallight = !manuallight;
                Serial.println("Manual light :" + String(manuallight));
            }
            else if (input < 20 && manuallight)
            {
                Serial.print("working on : " + String(input));
                if (input >= 10)
                {
                    // digitalWrite((input % 10) + 1, LOW);
                    switchManager((input % 10), 0);
                    Serial.print(" - " + String((input % 10) + 1) + " off");
                }
                else
                {
                    // digitalWrite(input + 1, HIGH);
                    switchManager(input, 1);
                    Serial.print(" - " + String(input + 1) + " on");
                }
            }
            else
            {
                Serial.print("-->" + String(input));
                // TestStream(input);
                BinaryManager(input);
            }
            Serial.println();
        }
    }
}