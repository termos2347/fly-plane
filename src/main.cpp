// приемник
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
        Serial.printf("📥 Джойстик1: X=%4d, Y=%4d, BTN1=%d\n", 
                     data.xAxis1, data.yAxis1, data.button1);
        Serial.printf("📥 Джойстик2: X=%4d, Y=%4d, BTN2=%d\n", 
                     data.xAxis2, data.yAxis2, data.button2);
        Serial.printf("🔄 Доп.кнопки: 0x%02X, CRC: %04X\n", 
                     data.buttons, data.crc);
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
  Serial.println("Сервоприводы для Джойстика 1:");
  Serial.println("  Y ось (влево-вправо):");
  Serial.println("    - Серво 0: GPIO 12");
  Serial.println("    - Серво 1: GPIO 27");
  Serial.println("  X ось (вперед-назад):");
  Serial.println("    - Серво 2: GPIO 13");
  Serial.println("    - Серво 3: GPIO 14");
  Serial.println("Сервоприводы для Джойстика 2:");
  Serial.println("  Y ось (влево-вправо):");
  Serial.println("    - Серво 4: GPIO 15");
  Serial.println("    - Серво 5: GPIO 16");
  Serial.println("  X ось (вперед-назад):");
  Serial.println("    - Серво 6: GPIO 17");
  Serial.println("    - Серво 7: GPIO 18");
  Serial.println("Индикация:");
  Serial.println("    - LED: GPIO 2");
  Serial.println("==============================");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("✈️ Запуск приемника управления с двумя джойстиками...");
  
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