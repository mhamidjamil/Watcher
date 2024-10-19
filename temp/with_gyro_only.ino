#include "Wire.h"
const int MPU_ADDR = 0x68;
int16_t accelerometer_x, accelerometer_y,
    accelerometer_z; // variables for accelerometer raw data
// int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
// int16_t temperature; // variables for temperature data

char tmp_str[7]; // temporary variable used in convert function
char *convert_int16_to_str(
    int16_t i) { // converts int16 to string. Moreover, resulting strings will
                 // have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}
int mainX = 0, mainY = 0;
void setup() {
  Serial.begin(115200);
  Serial.println(F("Activating Watcher..."));
  Wire.begin();
  Wire.beginTransmission(
      MPU_ADDR);    // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() { check_gy_sensor(true, 5); }
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
  // if (((change_Detector(mainX, global_X, neg_motion)) ||
  //      (change_Detector(mainY, global_Y, neg_motion))))
  if (neg_motion != -1) {
    if (((change_detector(accelerometer_x, mainX) +
          change_detector(accelerometer_y, mainY)) /
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
                     String(((change_detector(accelerometer_x, mainX) +
                              change_detector(accelerometer_y, mainY)) /
                             2)) +
                     ") ");
        // global_X = mainX;
        // global_Y = mainY;
        // global_Z = mainZ;
      }
      if (mainX != 0 || mainY != 0) {
        Serial.println(F(" Alert Send! "));
      }
    }
  }
  mainX = accelerometer_x;
  mainY = accelerometer_y;
}

bool change_Detector(int value_to_be_compare, int previous_value, int margin) {
  if ((value_to_be_compare >= previous_value - margin) &&
      (value_to_be_compare <= previous_value + margin)) {
    return false;
  } else {
    return true;
  }
}

int change_detector(int val1, int val2) {
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