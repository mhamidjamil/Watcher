int pin1 = 3; // efective on Sender pin: 1 and reciver pin: D2 (3rd)
int pin2 = 8; // efective on Sender pin: 2 and reciver pin: D3 (4th)
int pin3 = 7; // efective on Sender pin: 3 and reciver pin: D1 (2nd)
int pin4 = 5; // efective on Sender pin: 4 and reciver pin: D0 (1st)
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

void loop()
{
    if (Serial.available() > 0)
    {
        int input = Serial.parseInt();
        if (input > 0)
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
            Serial.println();
        }
    }
}