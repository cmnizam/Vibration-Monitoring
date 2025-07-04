#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL pins for ESP32

  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful");
  } else {
    Serial.println("MPU6050 connection failed");
    while (1);
  }
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  // Read accelerometer and gyroscope data
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Print accelerometer data
  Serial.print("aX: "); Serial.print(ax);
  Serial.print(" | aY: "); Serial.print(ay);
  Serial.print(" | aZ: "); Serial.println(az);

  // Print gyroscope data
  Serial.print("gX: "); Serial.print(gx);
  Serial.print(" | gY: "); Serial.print(gy);
  Serial.print(" | gZ: "); Serial.println(gz);

  Serial.println("------------------------------------");

  delay(500);
}