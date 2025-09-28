#include "Core/Types.h"
#include "Actuators/ServoManager.h"
#include "Communication/ESPNowManager.h"

ServoManager servoManager;
// Используем singleton вместо прямого экземпляра
ESPNowManager& espNow = ESPNowManager::getInstance();

void onControlDataReceived(const ControlData& data) {
    static unsigned long lastReceiveTime = 0;
    unsigned long currentTime = millis();
    
    // Вывод информации о полученных данных (не чаще 1 раза в секунду)
    static unsigned long lastPrint = 0;
    if (currentTime - lastPrint > 1000) {
        Serial.printf("📥 Получены данные: X=%4d, Y=%4d, BTN=%d\n", 
                     data.xAxis, data.yAxis, data.buttonPressed);
        lastPrint = currentTime;
    }
    
    // Обновляем время последнего получения данных
    lastReceiveTime = currentTime;
    
    // Передаем данные сервоприводам
    servoManager.update(data);
    
    // Индикация получения данных (быстрое мигание)
    digitalWrite(2, HIGH);
    delay(10);
    digitalWrite(2, LOW);
}

void printDeviceInfo() {
  Serial.println("✈️ ===== ИНФОРМАЦИЯ ПРИЕМНИКА =====");
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

void printPinConfiguration() {
  Serial.println("🔌 === КОНФИГУРАЦИЯ ПИНОВ ===");
  Serial.println("Сервоприводы:");
  Serial.println("  Y ось (влево-вправо):");
  Serial.println("    - Серво 0: GPIO 12");
  Serial.println("    - Серво 1: GPIO 27");
  Serial.println("  X ось (вперед-назад):");
  Serial.println("    - Серво 2: GPIO 13");
  Serial.println("    - Серво 3: GPIO 14");
  Serial.println("Индикация:");
  Serial.println("    - LED: GPIO 2");
  Serial.println("==============================");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("✈️ Запуск приемника управления самолетом...");
  
  // Вывод подробной информации об устройстве
  printDeviceInfo();
  
  // Вывод конфигурации пинов
  printPinConfiguration();
  
  // Настройка LED для индикации
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  
  // Инициализация компонентов
  Serial.println("🔧 Инициализация компонентов...");
  servoManager.begin();
  espNow.begin();
  espNow.registerCallback(onControlDataReceived);
  
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
  
  Serial.println("🚀 Приемник готов к работе");
  Serial.println("📡 Ожидание данных от передатчика...");
  Serial.println("💡 Светодиод будет мигать при получении данных");
  
  // Вывод MAC адреса для удобства копирования
  Serial.println("");
  Serial.println("=== ДЛЯ НАСТРОЙКИ ПЕРЕДАТЧИКА ===");
  Serial.print("MAC_ADDRESS = ");
  Serial.println(WiFi.macAddress());
  Serial.println("Скопируйте этот MAC в код передатчика");
  Serial.println("==================================");
  Serial.println("");
}

void loop() {
  // Основная логика обработки в callback функции
  
  // Медленное мигание в режиме ожидания
  static unsigned long lastBlink = 0;
  static bool waitingForData = true;
  
  if (waitingForData) {
    if (millis() - lastBlink > 2000) {
      digitalWrite(2, !digitalRead(2));
      lastBlink = millis();
      
      // Если долго нет данных, выводим сообщение
      static unsigned long lastMessage = 0;
      if (millis() - lastMessage > 10000) {
        Serial.println("⏳ Ожидание данных от передатчика...");
        Serial.print("MAC для подключения: ");
        Serial.println(WiFi.macAddress());
        lastMessage = millis();
      }
    }
  }
  
  delay(100);
}