// приемник
#include <esp_now.h>
#include <WiFi.h>

// Структура данных для управления
struct ControlData {
  int16_t throttle;
  int16_t rudder;
  int16_t elevator;
  int16_t ailerons;
  bool buttonPressed;
};

void printMacAddress() {
  Serial.println("=== ТЕСТ ПРИЕМНИКА ===");
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println("=====================");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  Serial.println("✅ Данные получены!");
  
  ControlData receivedData;
  memcpy(&receivedData, data, sizeof(receivedData));
  
  Serial.print("От: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  
  Serial.printf("Данные: Газ=%d, Руль=%d, Высота=%d, Элероны=%d, Кнопка=%d\n",
                receivedData.throttle,
                receivedData.rudder,
                receivedData.elevator,
                receivedData.ailerons,
                receivedData.buttonPressed);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Запуск приемника...");
  
  printMacAddress();
  
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Ошибка инициализации ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("✅ Приемник готов к тестированию");
}

void loop() {
  delay(1000);
  Serial.println("⏳ Ожидание данных...");
}