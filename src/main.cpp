// САМОЛЕТ (приемник)
#include <esp_now.h>
#include <WiFi.h>
#include "Core/Types.h"
#include "Actuators/ServoManager.h"
#include "Communication/ESPNowManager.h"

ServoManager servoManager;

// MAC адрес пульта (передатчика)
uint8_t transmitterMac[] = {0xEC, 0xE3, 0x34, 0x19, 0x23, 0xD4};

// Функция для добавления пира в ESP-NOW
bool addPeer(const uint8_t* macAddress) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, macAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    esp_err_t result = esp_now_add_peer(&peerInfo);
    if (result == ESP_OK) {
        Serial.println("✅ Пир успешно добавлен");
        return true;
    } else {
        Serial.printf("❌ Ошибка добавления пира: %d\n", result);
        return false;
    }
}

void onControlDataReceived(const ControlData& data) {
    // Вывод информации о полученных данных при каждом получении
    Serial.printf("📥 Получены данные! Джойстик1: X=%4d, Y=%4d\n", data.xAxis1, data.yAxis1);
    
    // Передаем данные сервоприводам
    servoManager.update(data);
    
    // Индикация получения данных (быстрое мигание)
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
}

void printDeviceInfo() {
  Serial.println("✈️ ===== ИНФОРМАЦИЯ САМОЛЕТА =====");
  Serial.print("MAC адрес: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getEfuseMac(), HEX);
  Serial.print("Частота CPU: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("Flash размер: ");
  Serial.print(ESP.getFlashChipSize() / (1024 * 1024));
  Serial.println(" MB");
  Serial.print("Свободная память: ");
  Serial.print(ESP.getFreeHeap() / 1024);
  Serial.println(" KB");
  Serial.println("===================================");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("✈️ Запуск системы управления самолетом...");
  
  // Вывод подробной информации об устройстве
  printDeviceInfo();
  
  // Настройка LED для индикации
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  
  // Инициализация компонентов
  Serial.println("🔧 Инициализация компонентов...");
  servoManager.begin();
  
  // Инициализация ESP-NOW в режиме приемника
  Serial.println("📡 Инициализация ESP-NOW...");
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Ошибка инициализации ESP-NOW");
    return;
  }
  
  // Регистрируем callback для приема данных
  esp_now_register_recv_cb([](const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len == sizeof(ControlData)) {
      ControlData data;
      memcpy(&data, incomingData, sizeof(ControlData));
      onControlDataReceived(data);
    }
  });
  
  // Добавляем пульт как пир
  Serial.println("⏳ Добавление пульта...");
  
  if (addPeer(transmitterMac)) {
    Serial.print("✅ Пульт добавлен: ");
    for(int i = 0; i < 6; i++) {
      Serial.print(transmitterMac[i], HEX);
      if(i < 5) Serial.print(":");
    }
    Serial.println();
  } else {
    Serial.println("❌ Не удалось добавить пульт");
  }
  
  // Калибровка сервоприводов
  Serial.println("🎯 Калибровка сервоприводов...");
  servoManager.calibrate();
  
  // Индикация готовности
  Serial.println("💡 Индикация готовности...");
  for(int i = 0; i < 5; i++) {
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
    delay(100);
  }
  
  Serial.println("🚀 Самолет готов к работе");
  Serial.println("📡 Ожидание команд с пульта...");
  Serial.print("MAC для подключения: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Медленное мигание в режиме ожидания
  static unsigned long lastBlink = 0;
  
  if (millis() - lastBlink > 2000) {
    digitalWrite(2, !digitalRead(2));
    lastBlink = millis();
    
    // Если долго нет данных, выводим сообщение
    static unsigned long lastMessage = 0;
    if (millis() - lastMessage > 5000) {
      if (esp_now_is_peer_exist(transmitterMac)) {
        Serial.println("✅ Пульт зарегистрирован в ESP-NOW, ожидание данных...");
      } else {
        Serial.println("❌ Пульт не зарегистрирован в ESP-NOW");
      }
      lastMessage = millis();
    }
  }
  
  delay(100);
}