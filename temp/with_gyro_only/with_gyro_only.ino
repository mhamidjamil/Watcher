#include "Wire.h"

// MPU6050 I2C address
const int MPU_ADDR = 0x68;

// Variables to store raw accelerometer data
int16_t accelerometer_x, accelerometer_y, accelerometer_z;
int mainX = 0, mainY = 0;

// Temporary buffer for string conversion
char tmp_str[7];

// Converts int16 to string for serial printing
char *convert_int16_to_str(int16_t i) {
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  Serial.println(F("Activating Watcher..."));

  // Initialize I2C communication with MPU6050
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register (wake up MPU6050)
  Wire.write(0);
  Wire.endTransmission(true);
}

void loop() {
  // Call the function to check for movement and alert
  check_gy_sensor(true, 5);
}

void check_gy_sensor(bool print_records, int neg_motion) {
  // Small delay to avoid excessive serial output
  delay(100);

  // Request accelerometer data from MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 7 * 2, true); // Request 14 registers

  // Read accelerometer data
  accelerometer_x = Wire.read() << 8 | Wire.read();
  accelerometer_y = Wire.read() << 8 | Wire.read();
  accelerometer_z = Wire.read() << 8 | Wire.read();

  // Print accelerometer values if print_records is true
  if (print_records) {
    Serial.print(F("--->aX = "));
    Serial.print(convert_int16_to_str(accelerometer_x));
    Serial.print(F(" | aY = "));
    Serial.print(convert_int16_to_str(accelerometer_y));
    Serial.print(F(" | aZ = "));
    Serial.println(convert_int16_to_str(accelerometer_z));
  }

  // Check for significant movement
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
        Serial.print(" avg change (" +
                     String(((change_detector(accelerometer_x, mainX) +
                              change_detector(accelerometer_y, mainY)) /
                             2)) +
                     ") ");
      }
      if (mainX != 0 || mainY != 0) {
        Serial.println(F(" Alert Send! "));
      }
    }
  }

  // Update mainX and mainY to current accelerometer values
  mainX = accelerometer_x;
  mainY = accelerometer_y;
}

// Function to detect significant changes in values
bool change_Detector(int value_to_be_compare, int previous_value, int margin) {
  return !((value_to_be_compare >= previous_value - margin) &&
           (value_to_be_compare <= previous_value + margin));
}

// Function to calculate absolute difference between two values
int change_detector(int val1, int val2) { return abs(val1 - val2); }
