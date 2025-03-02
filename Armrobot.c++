#include <Servo.h>
#include <Arduino.h>

// สร้างอ็อบเจกต์เซอร์โว
Servo servoBase, servoShoulder, servoElbow, servoWristRot, servoWristBend, servoGripper;

// 🕹️ กำหนดขา Joystick
#define JOY1_X A0  // ควบคุม Base
#define JOY1_Y A1  // ควบคุม Shoulder
#define JOY2_X A2  // ควบคุม Elbow
#define JOY2_Y A3  // ควบคุม Wrist Rotate
#define JOY3_X A4  // ควบคุม Wrist Bend

// 🛠️ ปุ่มควบคุม Gripper
#define BUTTON_OPEN 2     // ปุ่มเปิด Gripper
#define BUTTON_CLOSE 4    // ปุ่มปิด Gripper

// ขอบเขตองศาของเซอร์โว
#define MIN_ANGLE 0
#define MAX_ANGLE 180

// Deadzone สำหรับจอยสติ๊ก
#define DEADZONE 80

// 📝 ตำแหน่งเริ่มต้นของเซอร์โว
int posBase = 90, posShoulder = 90, posElbow = 90, posWristRot = 90, posWristBend = 90, posGripper = 90;

// 🌀 ฟังก์ชันเคลื่อนที่แบบสมูท
void smoothMove(Servo &servo, int &currentPos, int targetPos, int speedDelay) {
    if (currentPos != targetPos) {
        int step = (targetPos > currentPos) ? 1 : -1;
        currentPos += step;
        servo.write(currentPos); 
        delay(speedDelay);  // ความเร็วในการเคลื่อนที่ (ค่ามาก = ช้าลง)
    }
}

void setup() {
    servoBase.attach(5);
    servoShoulder.attach(3);
    servoElbow.attach(6);
    servoWristRot.attach(9);
    servoWristBend.attach(10);
    servoGripper.attach(11);

    pinMode(BUTTON_OPEN, INPUT_PULLUP);   // ปุ่มเปิด (ต่อกับ GND)
    pinMode(BUTTON_CLOSE, INPUT_PULLUP);  // ปุ่มปิด (ต่อกับ GND)

    servoGripper.write(posGripper);  // กำหนดตำแหน่งเริ่มต้น Gripper
}

void loop() {
    // อ่านค่าจาก Joystick
    int joy1X = analogRead(JOY1_X);
    int joy1Y = analogRead(JOY1_Y);
    int joy2X = analogRead(JOY2_X);
    int joy2Y = analogRead(JOY2_Y);  // ควบคุม Wrist Rotate
    int joy3X = analogRead(JOY3_X);

    // ปรับความเร็วเคลื่อนที่ (ค่ามาก = ช้าลง)
    int speedBase = map(abs(joy1X - 512), 0, 512, 20, 5);
    int speedShoulder = map(abs(joy1Y - 512), 0, 512, 20, 5);
    int speedElbow = map(abs(joy2X - 512), 0, 512, 20, 5);
    int speedWristRot = map(abs(joy2Y - 512), 0, 512, 30, 10);  // ✅ ช้าลงเป็นพิเศษ
    int speedWristBend = map(abs(joy3X - 512), 0, 512, 20, 5);

    // 🎮 ควบคุม Base
    if (abs(joy1X - 512) > DEADZONE) {
        int target = (joy1X > 512) ? posBase + 2 : posBase - 2;
        smoothMove(servoBase, posBase, constrain(target, MIN_ANGLE, MAX_ANGLE), speedBase);
    }

    // 🎮 ควบคุม Shoulder
    if (abs(joy1Y - 512) > DEADZONE) {
        int target = (joy1Y > 512) ? posShoulder + 2 : posShoulder - 2;
        smoothMove(servoShoulder, posShoulder, constrain(target, MIN_ANGLE, MAX_ANGLE), speedShoulder);
    }

    // 🎮 ควบคุม Elbow
    if (abs(joy2X - 512) > DEADZONE) {
        int target = (joy2X > 512) ? posElbow + 2 : posElbow - 2;
        smoothMove(servoElbow, posElbow, constrain(target, MIN_ANGLE, MAX_ANGLE), speedElbow);
    }

    // 🎮 ควบคุม Wrist Rotate
    if (abs(joy2Y - 512) > DEADZONE) {
        int target = (joy2Y > 512) ? posWristRot + 1 : posWristRot - 1;  // step 1 ช้าลง
        smoothMove(servoWristRot, posWristRot, constrain(target, MIN_ANGLE, MAX_ANGLE), speedWristRot);
    }

    // 🎮 ควบคุม Wrist Bend
    if (abs(joy3X - 512) > DEADZONE) {
        int target = (joy3X > 512) ? posWristBend + 2 : posWristBend - 2;
        smoothMove(servoWristBend, posWristBend, constrain(target, MIN_ANGLE, MAX_ANGLE), speedWristBend);
    }

    // 🛠️ ควบคุม Gripper ด้วยปุ่มกด
    if (digitalRead(BUTTON_OPEN) == LOW) {
        posGripper = constrain(posGripper + 90, MIN_ANGLE, MAX_ANGLE);  // เปิด +90°
        servoGripper.write(posGripper); 
        delay(300);  // ป้องกันการกดซ้ำเร็ว
        while (digitalRead(BUTTON_OPEN) == LOW);  // รอจนปล่อยปุ่ม
    }

    if (digitalRead(BUTTON_CLOSE) == LOW) {
        posGripper = constrain(posGripper - 90, MIN_ANGLE, MAX_ANGLE);  // ปิด -90°
        servoGripper.write(posGripper); 
        delay(300);  // ป้องกันการกดซ้ำเร็ว
        while (digitalRead(BUTTON_CLOSE) == LOW);  // รอจนปล่อยปุ่ม
    }

    delay(15);  // เพิ่มความสมูท
}
