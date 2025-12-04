/*
  Radar2450 - Минимальный пример
  Самый простой способ получения данных
*/

#include <Radar2450.h>

Radar2450 radar;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Простая инициализация
  if (radar.begin(16, 17)) {
    Serial.println("Радар готов!");
  }
}

void loop() {
  // 1. Обновляем данные
  radar.update();
  
  // 2. Получаем количество активных целей
  int count = radar.getActiveTargetCount();
  
  if (count > 0) {
    // 3. Для каждой цели выводим основные данные
    for (int i = 1; i <= count; i++) {
      Serial.printf("Цель %d: ", i);
      Serial.printf("ID=%d, ", radar.getTargetID(i));
      Serial.printf("Дист=%.1fm, ", radar.getTargetDistance(i));
      Serial.printf("Угол=%.0f°, ", radar.getTargetAngle(i));
      Serial.printf("Скор=%.1fкм/ч, ", radar.getTargetSpeedKmh(i));
      Serial.printf("%s\n", radar.getTargetMoving(i) ? "ДВИЖ" : "СТОП");
    }
  }
  
  delay(50);
}