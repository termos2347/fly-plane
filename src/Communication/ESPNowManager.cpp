#include "ESPNowManager.h"
#include <Arduino.h>

// Статическая переменная для доступа к экземпляру из статической функции
static ESPNowManager* espNowInstance = nullptr;

void ESPNowManager::begin() {
    WiFi.mode(WIFI_STA);
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ Ошибка инициализации ESP-NOW");
        return;
    }
    
    esp_now_register_recv_cb(onDataReceived);
    
    // Сохраняем указатель на экземпляр для статической функции
    espNowInstance = this;
    
    // Вывод MAC адреса для спаривания
    Serial.print("📡 MAC приемника: ");
    Serial.println(WiFi.macAddress());
    
    Serial.println("✅ ESP-NOW приемник готов");
}

void ESPNowManager::registerCallback(DataReceivedCallback callback) {
    dataCallback = callback;
}

void ESPNowManager::onDataReceived(const uint8_t* mac, const uint8_t* data, int len) {
    if (len != sizeof(ControlData)) {
        Serial.println("❌ Неверный размер пакета");
        return;
    }
    
    ControlData receivedData;
    memcpy(&receivedData, data, sizeof(receivedData));
    
    // Валидация CRC
    uint16_t calculatedCRC = 0;
    const uint8_t* bytes = (const uint8_t*)&receivedData;
    for(size_t i = 0; i < sizeof(ControlData) - sizeof(uint16_t); i++) {
        calculatedCRC += bytes[i];
    }
    
    if (calculatedCRC != receivedData.crc) {
        Serial.println("❌ Ошибка CRC");
        return;
    }
    
    // Вызов callback функции через экземпляр
    if (espNowInstance && espNowInstance->dataCallback != nullptr) {
        espNowInstance->dataCallback(receivedData);
    }
    
    // Диагностика
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500) {
        Serial.printf("📥 X: %4d, Y: %4d\n", receivedData.xAxis, receivedData.yAxis);
        lastPrint = millis();
    }
}

bool ESPNowManager::validateCRC(const ControlData& data) {
    // Та же логика, что и в передатчике
    uint16_t calculatedCRC = 0;
    const uint8_t* bytes = (const uint8_t*)&data;
    
    for(size_t i = 0; i < sizeof(ControlData) - sizeof(uint16_t); i++) {
        calculatedCRC += bytes[i];
    }
    
    return calculatedCRC == data.crc;
}

void ESPNowManager::sendDiagnostic() {
    // Можно добавить отправку диагностических данных
    Serial.println("📊 Диагностика ESP-NOW");
}