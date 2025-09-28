#pragma once
#include <Arduino.h>
#include "signal_receiver.h"
#include "servo_controller.h"
#include "motor_controller.h"

class FlightController {
private:
  ServoController& servoCtrl;
  MotorController& motorCtrl;
  ControlSignal lastSignal;
  
public:
  FlightController(ServoController& sc, MotorController& mc) 
    : servoCtrl(sc), motorCtrl(mc) {}
  
  void init() {
    Serial.println("Flight Controller initialized");
  }
  
  void processSignal(const ControlSignal& signal) {
    lastSignal = signal;
    
    // Проверка потери сигнала
    if(signal.signalLost) {
      emergencyProcedure();
      return;
    }
    
    // Обновление сервоприводов
    servoCtrl.updateFromSignal(signal);
    
    // Обновление двигателя
    motorCtrl.setThrottle(signal.throttle);
    
    // Логирование для отладки
    if(millis() % 1000 == 0) {
      printSignal(signal);
    }
  }
  
private:
  void emergencyProcedure() {
    // Процедура при потере сигнала
    motorCtrl.setThrottle(0); // Отключить двигатель
    Serial.println("EMERGENCY: Signal lost!");
  }
  
  void printSignal(const ControlSignal& sig) {
    Serial.print("T:");
    Serial.print(sig.throttle);
    Serial.print(" R:");
    Serial.print(sig.roll);
    Serial.print(" P:");
    Serial.print(sig.pitch);
    Serial.print(" Y:");
    Serial.println(sig.yaw);
  }
};