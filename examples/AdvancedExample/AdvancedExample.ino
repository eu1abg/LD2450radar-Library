/*
  Radar2450 - Продвинутый пример с фильтрацией
*/

#include <Radar2450.h>

Radar2450 radar;

// Структура для хранения истории целей
struct TargetHistory {
  int id;
  float maxSpeed;
  float totalDistance;
  unsigned long firstSeen;
};

TargetHistory targetHistory[10];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (radar.begin(16, 17)) {
    Serial.println("Радар инициализирован");
    
    // Настройки для точного отслеживания
    radar.setMaxDistance(6.0);
    radar.setSmoothingFactor(0.1);  // Сильное сглаживание
    radar.setMaxTargets(3);          // Только 3 главные цели
    radar.setAngleLimits(-45, 45);   // Узкий угол обзора
  }
}

void loop() {
  // Обновление данных
  int detected = radar.update();
  
  if (detected > 0) {
    // Получаем самую близкую цель
    if (radar.getActiveTargetCount() > 0) {
      displayClosestTarget();
      displayAllTargets();
      updateTargetHistory();
    }
  }
  
  delay(50);
}

// Функция отображения ближайшей цели
void displayClosestTarget() {
  Serial.println("\n=== БЛИЖАЙШАЯ ЦЕЛЬ ===");
  Serial.printf("ID: %d\n", radar.getTargetID(1));
  Serial.printf("Дистанция: %.2f м\n", radar.getTargetDistance(1));
  Serial.printf("Угол: %.1f°\n", radar.getTargetAngle(1));
  Serial.printf("Скорость: %.2f м/с (%.1f км/ч)\n", 
                radar.getTargetSpeed(1), radar.getTargetSpeedKmh(1));
  Serial.printf("Координаты: (%.2f, %.2f)\n", 
                radar.getTargetX(1), radar.getTargetY(1));
  Serial.printf("Движется: %s\n", 
                radar.getTargetMoving(1) ? "ДА" : "НЕТ");
}

// Функция отображения всех целей
void displayAllTargets() {
  int active = radar.getActiveTargetCount();
  Serial.printf("\n=== ВСЕ ЦЕЛИ (%d) ===\n", active);
  
  for (int i = 1; i <= active; i++) {
    Serial.printf("%d. ID:%d Дист:%.1fm Угол:%.0f° Скорость:%.1fкм/ч\n",
                  i,
                  radar.getTargetID(i),
                  radar.getTargetDistance(i),
                  radar.getTargetAngle(i),
                  radar.getTargetSpeedKmh(i));
  }
}

// Функция обновления истории целей
void updateTargetHistory() {
  int active = radar.getActiveTargetCount();
  
  for (int i = 1; i <= active; i++) {
    int id = radar.getTargetID(i);
    float speed = radar.getTargetSpeedKmh(i);
    
    // Ищем цель в истории
    bool found = false;
    for (int j = 0; j < 10; j++) {
      if (targetHistory[j].id == id) {
        // Обновляем максимальную скорость
        if (speed > targetHistory[j].maxSpeed) {
          targetHistory[j].maxSpeed = speed;
        }
        found = true;
        break;
      }
    }
    
    // Если новая цель - добавляем в историю
    if (!found) {
      for (int j = 0; j < 10; j++) {
        if (targetHistory[j].id == 0) {
          targetHistory[j].id = id;
          targetHistory[j].maxSpeed = speed;
          targetHistory[j].firstSeen = millis();
          break;
        }
      }
    }
  }
}