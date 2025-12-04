/*
  Radar2450 - Complete Example
  Простой пример получения всех данных с радара
  
  Подключение для ESP32:
  LD2450 TX -> GPIO16
  LD2450 RX -> GPIO17
  LD2450 3.3V -> 3.3V
  LD2450 GND -> GND
*/

#include <Radar2450.h>

// Пины подключения радара
#define RADAR_RX 16  // RX ESP32 <- TX LD2450
#define RADAR_TX 17  // TX ESP32 -> RX LD2450

// Создаем объект радара
Radar2450 radar;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=========================================");
  Serial.println("Radar2450 - Complete Data Example");
  Serial.println("=========================================");
  
  // Инициализация радара
  Serial.println("Инициализация радара...");
  
  if (radar.begin(RADAR_RX, RADAR_TX)) {
    Serial.println("✅ Радар инициализирован успешно!");
    
    // Настройка параметров (опционально)
    radar.setMaxDistance(8.0);        // Максимальная дистанция 8 метров
    radar.setSmoothingFactor(0.2);    // Коэффициент сглаживания 0.2
    radar.setMaxTargets(5);           // Отслеживать до 5 целей
    radar.setAngleLimits(-60, 60);    // Угол обзора от -60 до 60 градусов
    
    Serial.println("\nНастройки:");
    Serial.println("  Макс. дистанция: 8.0 м");
    Serial.println("  Сглаживание: 0.2");
    Serial.println("  Макс. целей: 5");
    Serial.println("  Угол обзора: -60° до 60°");
    Serial.println("\nГотов к работе!");
    Serial.println("=========================================\n");
    
  } else {
    Serial.println("❌ Ошибка инициализации радара!");
    Serial.println("Проверьте подключение:");
    Serial.println("  LD2450 TX -> GPIO" + String(RADAR_RX));
    Serial.println("  LD2450 RX -> GPIO" + String(RADAR_TX));
    Serial.println("  LD2450 3.3V -> 3.3V");
    Serial.println("  LD2450 GND -> GND");
    while(1); // Остановка при ошибке
  }
}

void loop() {
  // =========================================
  // 1. ОБНОВЛЕНИЕ ДАННЫХ - update()
  // =========================================
  // Обновляем данные радара, получаем количество обнаруженных целей
  int detectedTargets = radar.update();
  
  // Если обнаружены цели
  if (detectedTargets > 0) {
    
    // =========================================
    // 2. КОЛИЧЕСТВО АКТИВНЫХ ЦЕЛЕЙ - getActiveTargetCount()
    // =========================================
    int activeTargets = radar.getActiveTargetCount();
    
    Serial.println("╔══════════════════════════════════════════╗");
    Serial.printf("║ Обнаружено целей: %d | Активных: %d      ║\n", detectedTargets, activeTargets);
    Serial.println("╚══════════════════════════════════════════╝");
    
    // =========================================
    // 3. ОБРАБОТКА КАЖДОЙ АКТИВНОЙ ЦЕЛИ
    // =========================================
    for (int i = 1; i <= activeTargets; i++) {
      Serial.println("\n──────────────────────────────────────────");
      Serial.printf("Цель #%d (позиция в списке)\n", i);
      Serial.println("──────────────────────────────────────────");
      
      // =========================================
      // 4. ID ЦЕЛИ - getTargetID()
      // =========================================
      int targetID = radar.getTargetID(i);
      Serial.printf("  ID цели: %d\n", targetID);
      
      // =========================================
      // 5. ДИСТАНЦИЯ - getTargetDistance()
      // =========================================
      float distance = radar.getTargetDistance(i);
      Serial.printf("  Дистанция: %.2f м\n", distance);
      
      // =========================================
      // 6. УГОЛ - getTargetAngle()
      // =========================================
      float angle = radar.getTargetAngle(i);
      Serial.printf("  Угол: %.1f°\n", angle);
      
      // =========================================
      // 7-8. КООРДИНАТЫ - getTargetX(), getTargetY()
      // =========================================
      float x = radar.getTargetX(i);
      float y = radar.getTargetY(i);
      Serial.printf("  Координаты: X=%.2f м, Y=%.2f м\n", x, y);
      
      // =========================================
      // 9-10. СКОРОСТЬ - getTargetSpeed(), getTargetSpeedKmh()
      // =========================================
      float speedMps = radar.getTargetSpeed(i);
      float speedKmh = radar.getTargetSpeedKmh(i);
      Serial.printf("  Скорость: %.2f м/с (%.1f км/ч)\n", speedMps, speedKmh);
      
      // =========================================
      // 11. СТАТУС ДВИЖЕНИЯ - getTargetMoving()
      // =========================================
      bool isMoving = radar.getTargetMoving(i);
      Serial.printf("  Состояние: %s\n", isMoving ? "ДВИЖЕТСЯ" : "НЕПОДВИЖНА");
      
      // =========================================
      // ДОПОЛНИТЕЛЬНАЯ ИНФОРМАЦИЯ
      // =========================================
      // Определяем сектор
      String sector = "Центр";
      if (angle < -30) sector = "Лево";
      else if (angle > 30) sector = "Право";
      else if (angle < -10) sector = "Лево-центр";
      else if (angle > 10) sector = "Право-центр";
      
      // Определяем зону по дистанции
      String zone = "Близко";
      if (distance > 5.0) zone = "Далеко";
      else if (distance > 2.0) zone = "Средняя";
      
      Serial.printf("  Сектор: %s, Зона: %s\n", sector.c_str(), zone.c_str());
    }
    
    Serial.println("\n══════════════════════════════════════════");
    
  } else {
    // Нет обнаруженных целей
    Serial.println("Целей не обнаружено");
  }
  
  // =========================================
  // 12. ДОПОЛНИТЕЛЬНЫЕ ФУНКЦИИ
  // =========================================
  // Каждые 10 секунд выводим отладочную информацию
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 10000) {
    Serial.println("\n══════════════════════════════════════════");
    Serial.println("ОТЛАДОЧНАЯ ИНФОРМАЦИЯ:");
    Serial.println("══════════════════════════════════════════");
    
    // Выводим полную отладочную информацию
    radar.printDebugInfo();
    
    // Частота обновления
    float updateRate = radar.getUpdateRateHz();
    Serial.printf("Частота обновления: %.1f Гц\n", updateRate);
    
    lastDebug = millis();
  }
  
  // Задержка между обновлениями (рекомендуется 50 мс = 20 Гц)
  delay(50);
}