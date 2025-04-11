/**
 * @file main.cpp
 * @brief Demo điều khiển động cơ ESP32
 * @version 0.1
 * @date 2025-04-10
 * @copyright Copyright (c) 2025
 */

#include <Arduino.h>
// ====== Chân động cơ ======
#define IN1 2    // PWM động cơ phải
#define IN2 0    // Chiều động cơ phải
#define IN3 4    // PWM động cơ trái
#define IN4 16   // Chiều động cơ trái

// ====== Chân encoder ======
#define en_rA 17   // Encoder phải: A
#define en_rB 5    // Encoder phải: B
#define en_lA 22   // Encoder trái: A
#define en_lB 23   // Encoder trái: B

// ====== Biến encoder ======
volatile int en_rcount = 0; // Số xung encoder phải
volatile int en_lcount = 0; // Số xung encoder trái

// ====== Biến lưu trạng thái trước của encoder ======
// use for ISR
volatile int last_rA = 0;
volatile int last_rB = 0;
volatile int last_lA = 0;
volatile int last_lB = 0;

// ====== Biến xuất PID (nếu cần) ======
int dt = 20; // Thời gian giữa các lần đọc encoder (ms)
int setPoint = 3000; // Giá trị mong muốn cho động cơ (số xung)


// right encoder
float Kp_r = 40; // Hệ số PID cho động cơ phải
float Ki_r = 0.005; // Hệ số PID cho động cơ phải
float Kd_r = 10; // Hệ số PID cho động cơ phải

// left encoder
float Kp_l = 40; // Hệ số PID cho động cơ trái
float Ki_l = 0.005; // Hệ số PID cho động cơ trái
float Kd_l = 10; // Hệ số PID cho động cơ trái




// ====== Encoder ISR ======
void IRAM_ATTR encoderISR_right();
void IRAM_ATTR encoderISR_left();

// Hàm PID và Monitor (chưa có nội dung cụ thể)
int motor_control_r(int, int, int);   // Nếu bạn chưa dùng, có thể xóa hoặc comment
int motor_control_l(int, int, int);   // Nếu bạn chưa dùng, có thể xóa hoặc comment
void monitor();

// funcontion control motor
void motor_control(int, int); // Hàm điều khiển động cơ



void setup() {
  Serial.begin(115200);

  // Cấu hình chân động cơ
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Khởi tạo mức LOW cho động cơ
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Cấu hình chân encoder với PullUp nội bộ
  pinMode(en_rA, INPUT_PULLUP);
  pinMode(en_rB, INPUT_PULLUP);
  pinMode(en_lA, INPUT_PULLUP);
  pinMode(en_lB, INPUT_PULLUP);

  // Đọc giá trị ban đầu của các chân encoder
  last_rA = digitalRead(en_rA);
  last_rB = digitalRead(en_rB);
  last_lA = digitalRead(en_lA);
  last_lB = digitalRead(en_lB);

  // Attach interrupt cho các chân encoder
  attachInterrupt(digitalPinToInterrupt(en_rA), encoderISR_right, CHANGE);
  attachInterrupt(digitalPinToInterrupt(en_rB), encoderISR_right, CHANGE);
  attachInterrupt(digitalPinToInterrupt(en_lA), encoderISR_left, CHANGE);
  attachInterrupt(digitalPinToInterrupt(en_lB), encoderISR_left, CHANGE);
}
unsigned long time_m = millis();
void loop() 
{
  int PWM_r = 0;
  int PWM_l = 0;
  unsigned long currentTime = millis();
  if(currentTime - time_m < 3000)
  {
    PWM_r = motor_control_r(setPoint, en_rcount, dt);
    PWM_l = motor_control_l(setPoint, en_lcount, dt);
    motor_control(PWM_r, PWM_l); // Gọi hàm điều khiển động cơ
  }
  else
  {
    en_rcount = 0; // Đặt lại số xung encoder phải
    en_lcount = 0; // Đặt lại số xung encoder trái
    time_m = millis(); // Cập nhật thời gian
  }


  // motor_control(PWM_r, PWM_l);
  Serial.print("PWM Right: ");
  Serial.print(PWM_r);
  Serial.print(" | PWM Left: ");
  Serial.println(PWM_l);

  monitor(); // Gọi hàm giám sát encoder
  // delay(1000);



}


// Hàm giám sát encoder và output động cơ
void monitor() {
  Serial.print("Right Encoder: ");
  Serial.print(en_rcount);
  Serial.print(" | Left Encoder: ");
  Serial.println(en_lcount);
  // Serial.print(" | PID output right: ");
  // Serial.print(temp_r);
  // Serial.print(" | PID output left: ");
  // Serial.println(temp_l);
  // Serial.print(" | Out Right: ");
  // Serial.print(output_r);
  // Serial.print(" | Out Left: ");
  // Serial.println(output_l);
  //delay(1000);
}

// Hàm ngắt xử lý encoder bên phải
void IRAM_ATTR encoderISR_right() {  
  int A = digitalRead(en_rA);
  int B = digitalRead(en_rB);

  if (A != last_rA) 
  {  // Nếu A thay đổi trước
    if (A == B)
      en_rcount++;  // Quay thuận
    else
      en_rcount--;  // Quay ngược
  } 
  else 
  {  // Nếu B thay đổi trước
    if (A == B)
      en_rcount--;  // Quay ngược
    else
      en_rcount++;  // Quay thuận
  }

  last_rA = A;
  last_rB = B;
}

// Hàm ngắt xử lý encoder bên trái
void IRAM_ATTR encoderISR_left() {
  int A = digitalRead(en_lA);
  int B = digitalRead(en_lB);

  if (A != last_lA) 
  {  // Nếu A thay đổi trước
    if (A == B)
      en_lcount++;  // Quay thuận
    else
      en_lcount--;  // Quay ngược
  } 
  else 
  {  // Nếu B thay đổi trước
    if (A == B)
      en_lcount--;  // Quay ngược
    else
      en_lcount++;  // Quay thuận
  }

  last_lA = A;
  last_lB = B;
}

int motor_control_r(int setPoint, int currentCount_r, int dt)
{
  //right motor
  static float intergral_r = 0;
  static float previousError_r = 0;

  float error_r = setPoint - currentCount_r; // calculate error
  intergral_r += error_r * dt; // calculate integral
  float derivative_r = (error_r - previousError_r) / dt; // calculate derivative

  int output_r = Kp_r * error_r + Ki_r * intergral_r + Kd_r * derivative_r; // calculate output
  
  if(abs(error_r) < 100)
  {
    output_r = constrain(output_r, -100, 100); // limit output to -100 to 100
  }

  if(abs(error_r) < 3 && abs(output_r) < 50)
  {
    output_r = 0; // limit output to -50 to 50
  }
  
  output_r = constrain(output_r, -255, 255); // limit output to -255 to 255
  
  previousError_r = error_r; // update previous error

  return output_r;
}
int motor_control_l(int setPoint, int currentCount_l, int dt)
{
  static float intergral_l = 0;
  static float previousError_l = 0;

  float error_l = setPoint - currentCount_l; // calculate error
  intergral_l += error_l * dt; // calculate integral
  float derivative_l = (error_l - previousError_l) / dt; // calculate derivative

  int output_l = Kp_l * error_l + Ki_l * intergral_l + Kd_l * derivative_l; // calculate output
  
  if(abs(error_l) < 100)
  {
    output_l = constrain(output_l, -100, 100); // limit output to -100 to 100
  }

  if(abs(error_l) < 3 && abs(output_l) < 50)
  {
    output_l = 0; // limit output to -50 to 50
  }
  
  output_l = constrain(output_l, -255, 255); // limit output to -255 to 255
  
  previousError_l = error_l; // update previous error

  return output_l;
}
void motor_control(int PWM_r, int PWM_l)
{
  // Điều khiển động cơ bên trai
  if(PWM_l >0)
  {
    analogWrite(IN2, PWM_l);
    analogWrite(IN1, LOW);
  }
  else if(PWM_l < 0)
  {
    analogWrite(IN2, LOW);
    analogWrite(IN1, abs(PWM_l));
  }
  
  // Điều khiển động cơ phai
  if(PWM_r >0)
  {
    analogWrite(IN3, PWM_r);
    analogWrite(IN4, LOW);
  }
  else if(PWM_r < 0)
  {
    analogWrite(IN3, LOW);
    analogWrite(IN4, abs(PWM_r));
  }

}