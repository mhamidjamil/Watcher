int pin1 = 16;
int pin2 = 5;
int pin3 = 0;
int pin4 = 2;
int globPin1Value = 0;
int globPin2Value = 0;
int globPin3Value = 0;
int globPin4Value = 0;
void setup()
{
    Serial.begin(115200);
    pinMode(pin1, INPUT);
    pinMode(pin2, INPUT);
    pinMode(pin3, INPUT);
    pinMode(pin4, INPUT);
    // pinMode(VT_pin, OUTPUT);
    // pinMode(indicator, OUTPUT);
    // resetReciver(1);
}
void RFmsgDecoder(int msgCode)
{
    switch (msgCode)
    {
    case 1:
        // Serial.println("1");
        Serial.println("Motion detected on gyro sensor");
        break;
    case 2:
        // Serial.println("2");
        Serial.println("Motion detected on ultrasonic sensor");
        break;
    case 3:
        // Serial.println("3");
        Serial.println("Motion detected on both ultrasonic and gyro sensor");
        break;
    case 4:
        Serial.println("4");
        break;
    case 5:
        Serial.println("5");
        break;
    case 6:
        Serial.println("6");
        break;
    case 7:
        Serial.println("7");
        break;
    case 8:
        Serial.println("8");
        break;
    case 9:
        Serial.println("9");
        break;
    case 10:
        Serial.println("10");
        break;
    case 11:
        Serial.println("11");
        break;
    case 12:
        Serial.println("12");
        break;
    case 13:
        Serial.println("13");
        break;
    case 14:
        Serial.println("14");
        break;
    case 15:
        Serial.println("15");
        break;
    default:
        Serial.println("Msg out of range");
        break;
    }
}
void bineryToDecimal(int pin1, int pin2, int pin3, int pin4)
{
    int decimal = 0;
    if (pin1 == 1)
    {
        decimal += 8;
    }
    if (pin2 == 1)
    {
        decimal += 4;
    }
    if (pin3 == 1)
    {
        decimal += 2;
    }
    if (pin4 == 1)
    {
        decimal += 1;
    }
    Serial.println(decimal);
    RFmsgDecoder(decimal);
}
void loop()
{
    int globPin1ValueTemp = digitalRead(pin1);
    int globPin2ValueTemp = digitalRead(pin2);
    int globPin3ValueTemp = digitalRead(pin3);
    int globPin4ValueTemp = digitalRead(pin4);
    if (globPin1ValueTemp != globPin1Value || globPin2ValueTemp != globPin2Value || globPin3ValueTemp != globPin3Value || globPin4ValueTemp != globPin4Value)
    {
        globPin1Value = globPin1ValueTemp;
        globPin2Value = globPin2ValueTemp;
        globPin3Value = globPin3ValueTemp;
        globPin4Value = globPin4ValueTemp;
        // digitalWrite(indicator, HIGH);
        // Serial.println("->" + String(globPin1ValueTemp) + "," + String(globPin2ValueTemp) + "," + String(globPin3ValueTemp) + "," + String(globPin4ValueTemp) + ".");
        if (globPin1Value == 1 || globPin2Value == 1 || globPin3Value == 1 || globPin4Value == 1)
        {
            bineryToDecimal(globPin1Value, globPin2Value, globPin3Value, globPin4Value);
        }

        // delay(500);
        // digitalWrite(indicator, LOW);
    }
    // delay(50);
}