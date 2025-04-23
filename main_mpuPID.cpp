#include <Arduino.h>
#include <Wire.h>
#include <math.h>

// ====== MPU6050 ======
#define MPU_ADDR 0x68
int16_t gyroZ;
float gyroZ_offset = 0;
float yaw = 0;
unsigned long lastTime = 0;
float yaw_drift_comp = 0.001; // ⚙️ hệ số bù drift: dương nếu yaw trôi phải, âm nếu trôi trái

// ====== Motor Pins ======
#define IN1 2
#define IN2 0
#define IN3 4
#define IN4 16

// Thời gian PID (ms)
const int dt = 20;

// ====== Hiệu chỉnh độ lệch gyro ======
void calibrateGyro() {
  long sum = 0;
  int n = 500;
  for (int i = 0; i < n; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x47);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_ADDR, (size_t)2, (bool)true);
    int16_t gz = Wire.read() << 8 | Wire.read();
    sum += gz;
    delay(2);
  }
  gyroZ_offset = sum / (float)n;
  Serial.print("GyroZ Offset: ");
  Serial.println(gyroZ_offset);
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  Wire.begin(33, 32); // SDA=33, SCL=32

  // Khởi tạo MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("MPU6050 OK");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  calibrateGyro();
  lastTime = millis();
}

// ====== Đọc yaw từ MPU6050 ======
float mpu() {
  unsigned long now = millis();
  float deltaT = (now - lastTime) / 1000.0;
  lastTime = now;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (size_t)2, (bool)true);
  if (Wire.available() == 2) {
    gyroZ = Wire.read() << 8 | Wire.read();
  } else {
    Serial.println("Lỗi đọc gyroZ");
    return yaw;
  }

  float gyroZ_dps = (gyroZ - gyroZ_offset) / 131.0;

  // ✅ Bù sai số trôi bằng hệ số tùy chỉnh
  float compensated_gyro = gyroZ_dps + yaw_drift_comp;

  yaw += compensated_gyro * deltaT;

  Serial.print("Yaw: ");
  Serial.print(yaw);
  Serial.print(" | Drift comp: ");
  Serial.println(yaw_drift_comp);

  return yaw;
}

// ====== PID điều chỉnh hướng ======
float pid_yaw(float currentYaw, float deltaT) {
  static float previous_error = 0;
  static float integral = 0;

  float Kp = 15;
  float Ki = 0.1;
  float Kd = 1;

  float error = 0.0 - currentYaw;
  integral += error * deltaT;
  float derivative = (error - previous_error) / deltaT;

  float output = Kp * error + Ki * integral + Kd * derivative;
  previous_error = error;

  return output;
}

// ====== Điều khiển động cơ ======
void motor_control(int PWM_r, int PWM_l) {
  PWM_r = constrain(PWM_r, -255, 255);
  PWM_l = constrain(PWM_l, -255, 255);

  if (PWM_l > 0) {
    analogWrite(IN2, PWM_l);
    analogWrite(IN1, LOW);
  } else {
    analogWrite(IN2, LOW);
    analogWrite(IN1, abs(PWM_l));
  }

  if (PWM_r > 0) {
    analogWrite(IN3, PWM_r);
    analogWrite(IN4, LOW);
  } else {
    analogWrite(IN3, LOW);
    analogWrite(IN4, abs(PWM_r));
  }
}

// ====== Vòng lặp chính ======
void loop() {
  // ⌨️ Cho phép chỉnh hệ số bù yaw bằng phím Serial
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '+') yaw_drift_comp += 0.1;
    if (c == '-') yaw_drift_comp -= 0.1;
  }

  float currentYaw = mpu();
  float correction = pid_yaw(currentYaw, dt / 1000.0);

  int PWM_base = 0;
  int PWM_r = PWM_base + correction;
  int PWM_l = PWM_base - correction;

  PWM_r = constrain(PWM_r, -255, 255);
  PWM_l = constrain(PWM_l, -255, 255);

  motor_control(PWM_r, PWM_l);

  delay(dt);
}
