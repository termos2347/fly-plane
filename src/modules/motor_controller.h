#pragma once
#include <Arduino.h>

class MotorController {
private:
  int motorPin;
  
public:
  void init(int pin = 13) {
    motorPin = pin;
    ledcSetup(0, 50, 16); // Канал 0, 50Hz, 16-bit resolution
    ledcAttachPin(motorPin, 0);
    Serial.println("Motor Controller initialized");
  }
  
  void setThrottle(int throttle) {
    // Преобразование 0-180 в ШИМ сигнал
    int pwmValue = map(throttle, 0, 180, 1000, 2000); // 1000-2000 микросекунды
    int dutyCycle = map(pwmValue, 1000, 2000, 3276, 6553); // Для 16-bit
    
    ledcWrite(0, dutyCycle);
    
    Serial.print("Motor throttle: ");
    Serial.println(throttle);
  }
};