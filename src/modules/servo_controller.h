#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

class ServoController {
private:
  Servo servos[8]; // До 8 сервоприводов
  
public:
  void init() {
    // Инициализация серво
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    for(int i = 0; i < 8; i++) {
      servos[i].setPeriodHertz(50); // Стандартная частота серво
    }
    Serial.println("Servo Controller initialized");
  }
  
  void setServoAngle(int servoNum, int angle) {
    if(servoNum >= 0 && servoNum < 8) {
      // Преобразование 0-180 в микросекунды (500-2400)
      int pulseWidth = map(angle, 0, 180, 500, 2400);
      servos[servoNum].attach(servoNum + 2); // Пины 2-9 для серво
      servos[servoNum].writeMicroseconds(pulseWidth);
    }
  }
  
  void updateFromSignal(const ControlSignal& signal) {
    // Маппинг каналов на серво
    setServoAngle(0, signal.roll);    // Элероны
    setServoAngle(1, signal.pitch);   // Рули высоты
    setServoAngle(2, signal.yaw);     // Рули направления
    setServoAngle(3, signal.aux1);    // Вспомогательные
  }
};