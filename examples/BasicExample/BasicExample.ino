#include <LD2450radar.h>

LD2450radar radar(Serial1, 16, 17); // RX=16, TX=17 (ESP32)

void setup() {
    Serial.begin(115200);
    radar.begin(); // Инициализация
}

void loop() {
    radar.update(); // Обновление данных
    
    int active = radar.getActiveCount();
    if (active > 0) {
        Serial.print("Активных целей: ");
        Serial.println(active);
        
        // Данные первой активной цели
        Serial.print("Дистанция: ");
        Serial.print(radar.getDistance(1));
        Serial.print(" м, Скорость: ");
        Serial.print(radar.getSpeed(1));
        Serial.println(" м/с");
    }
    delay(50); // ~20 Гц
}