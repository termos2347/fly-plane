#pragma once
#include <Arduino.h>

// Структура для унифицированного представления сигнала
struct ControlSignal {
  int throttle;  // 0-180
  int roll;      // 0-180  
  int pitch;     // 0-180
  int yaw;       // 0-180
  int aux1;      // 0-180
  int aux2;      // 0-180
  bool signalLost = false;
};

class SignalReceiver {
public:
  void init() {
    // Инициализация приемника (заглушка)
    Serial.println("Signal Receiver initialized");
  }
  
  ControlSignal getSignal() {
    ControlSignal signal;
    
    // Здесь будет реальный код приема сигнала
    // Пока возвращаем тестовые значения
    signal.throttle = 180;
    signal.roll = 180;
    signal.pitch = 180; 
    signal.yaw = 180;
    signal.aux1 = 180;
    signal.aux2 = 180;
    signal.signalLost = false;
    
    // Для тестирования можно раскомментировать:
    // signal = readPPM();
    // signal = readSBUS();
    // signal = readWiFi();
    
    return signal;
  }
  
private:
  // Примеры методов для разных протоколов
  ControlSignal readPPM() {
    // Реализация PPM декодирования
    ControlSignal sig;
    // ... код
    return sig;
  }
  
  ControlSignal readSBUS() {
    // Реализация SBUS декодирования  
    ControlSignal sig;
    // ... код
    return sig;
  }
  
  ControlSignal readWiFi() {
    // Реализация приема по WiFi
    ControlSignal sig;
    // ... код
    return sig;
  }
};