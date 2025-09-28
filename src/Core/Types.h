#pragma once
#include <cstdint>

// Структура данных для управления (должна быть идентична передатчику)
struct ControlData {
    int16_t xAxis;      // Ось X джойстика (-512 до +512)
    int16_t yAxis;      // Ось Y джойстика (-512 до +512)
    bool buttonPressed; // Кнопка джойстика
    uint16_t crc;       // Контрольная сумма
};

// Конфигурация пинов для приемника
struct HardwareConfig {
    // Сервоприводы
    static const uint8_t SERVO_0_PIN = 12;  // Y ось
    static const uint8_t SERVO_1_PIN = 27;  // Y ось  
    static const uint8_t SERVO_2_PIN = 13;  // X ось
    static const uint8_t SERVO_3_PIN = 14;  // X ось
    static const uint8_t LED_PIN = 2;
};