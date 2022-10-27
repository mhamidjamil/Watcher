int pin1 = 16;
int pin2 = 5;
int pin3 = 0;
int pin4 = 2;
int globPin1Value = 0;
int globPin2Value = 0;
int globPin3Value = 0;
int globPin4Value = 0;
int VT_pin = 14;
int indicator = LED_BUILTIN;
void resetReciver(int mode);
void setup()
{
    Serial.begin(9600);
    pinMode(pin1, INPUT);
    pinMode(pin2, INPUT);
    pinMode(pin3, INPUT);
    pinMode(pin4, INPUT);
    pinMode(VT_pin, OUTPUT);
    pinMode(indicator, OUTPUT);
    // resetReciver(1);
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
        Serial.println("->" + String(globPin1ValueTemp) + "," + String(globPin2ValueTemp) + "," + String(globPin3ValueTemp) + "," + String(globPin4ValueTemp) + ".");
        // delay(500);
        // digitalWrite(indicator, LOW);
    }
    // delay(50);
}
void resetReciver(int mode)
{
    Serial.println("Reseting Reciver");
    digitalWrite(VT_pin, LOW);

    for (int i = 1; i <= 8; i++)
    {
        digitalWrite(VT_pin, HIGH);
        delay(300);
        digitalWrite(VT_pin, LOW);
        delay(300);
    }
    delay(4000);
    for (; mode > 0; mode--)
    {
        digitalWrite(VT_pin, HIGH);
        delay(300);
        digitalWrite(VT_pin, LOW);
        delay(300);
    }
}