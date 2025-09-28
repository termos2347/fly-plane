#include "ServoManager.h"
#include <Arduino.h>

void ServoManager::initializeServos() {
    // Инициализация сервоприводов в конструкторе или отдельном методе
    servos[0] = {Servo(), 12, false, 1000, 2000}; // Servo 0 - Y ось
    servos[1] = {Servo(), 27, false, 1000, 2000}; // Servo 1 - Y ось  
    servos[2] = {Servo(), 13, false, 1000, 2000}; // Servo 2 - X ось
    servos[3] = {Servo(), 14, false, 1000, 2000}; // Servo 3 - X ось
}

void ServoManager::begin() {
    Serial.println("🔧 Инициализация сервоприводов...");
    
    initializeServos();
    
    // Разрешаем выделение таймеров для ESP32
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    for(int i = 0; i < 4; i++) {
        servos[i].servo.setPeriodHertz(50); // Стандартная частота серв
        servos[i].servo.attach(servos[i].pin, 
                              servos[i].minPulse, 
                              servos[i].maxPulse);
        servos[i].servo.write(90); // Нейтральное положение
        delay(100);
    }
    
    Serial.println("✅ Сервоприводы инициализированы");
}

void ServoManager::update(const ControlData& data) {
    int16_t xOutput, yOutput;
    applyMixer(data, xOutput, yOutput);
    
    // Управление сервоприводами X оси (пины 13, 14)
    servos[2].servo.writeMicroseconds(mapToPulse(xOutput, servos[2].reversed));
    servos[3].servo.writeMicroseconds(mapToPulse(xOutput, servos[3].reversed));
    
    // Управление сервоприводами Y оси (пины 12, 27)  
    servos[0].servo.writeMicroseconds(mapToPulse(yOutput, servos[0].reversed));
    servos[1].servo.writeMicroseconds(mapToPulse(yOutput, servos[1].reversed));
}

void ServoManager::applyMixer(const ControlData& data, int16_t& xOutput, int16_t& yOutput) {
    // Простое преобразование без микширования
    // X ось: вперед-назад
    xOutput = data.xAxis;
    
    // Y ось: влево-вправо  
    yOutput = data.yAxis;
    
    // Ограничение значений
    xOutput = constrain(xOutput, -512, 512);
    yOutput = constrain(yOutput, -512, 512);
}

int ServoManager::mapToPulse(int16_t value, bool reverse) {
    if (reverse) {
        value = -value;
    }
    
    // Преобразование -512..512 в 1000..2000 мкс
    return map(value, -512, 512, 1000, 2000);
}

void ServoManager::setReverse(uint8_t servoIndex, bool reverse) {
    if (servoIndex < 4) {
        servos[servoIndex].reversed = reverse;
    }
}

void ServoManager::calibrate() {
    Serial.println("🎯 Калибровка сервоприводов...");
    
    // Установка нейтрального положения
    for(int i = 0; i < 4; i++) {
        servos[i].servo.writeMicroseconds(1500);
    }
    delay(1000);
    
    Serial.println("✅ Калибровка завершена");
}