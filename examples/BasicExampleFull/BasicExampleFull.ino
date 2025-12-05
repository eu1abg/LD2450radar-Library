#include <LD2450radar.h>

// Создаем объект радара на Serial1, пины 16(RX), 17(TX)
LD2450radar radar(Serial1, 16, 17);

void setup() {
    Serial.begin(115200);
    radar.begin(); // Инициализация радара
    
    Serial.println("=== LD2450 Radar Demo ===");
    Serial.println("Ожидание данных с радара...");
    Serial.println();
}

void loop() {
    // Обновляем данные с радара
    int rawTargets = radar.update();
    
    // Если есть активные цели
    if (radar.getActiveCount() > 0) {
        // Получаем ВСЕ данные для первой (ближайшей) активной цели
        
        // 1. Основные данные
        int id = radar.getID(1);           // ID цели
        float distance = radar.getDistance(1);  // Дистанция (метры)
        float angle = radar.getAngle(1);   // Угол (градусы)
        float speed = radar.getSpeed(1);   // Скорость (м/с)
        
        // 2. Декартовы координаты
        float x = radar.getX(1);           // Координата X (метры)
        float y = radar.getY(1);           // Координата Y (метры)
        
        // 3. Производные данные
        float speedKmh = speed * 3.6f;     // Скорость в км/ч
        
        // Выводим ВСЁ в Serial
        Serial.println("=== ДАННЫЕ ЦЕЛИ ===");
        
        // Основные параметры
        Serial.print("ID: ");
        Serial.println(id);
        
        Serial.print("Дистанция: ");
        Serial.print(distance, 2);  // 2 знака после запятой
        Serial.println(" м");
        
        Serial.print("Угол: ");
        Serial.print(angle, 1);     // 1 знак после запятой
        Serial.println("°");
        
        // Положение
        Serial.print("Координаты: X=");
        Serial.print(x, 2);
        Serial.print(" м, Y=");
        Serial.print(y, 2);
        Serial.println(" м");
        
        // Скорость
        Serial.print("Скорость: ");
        Serial.print(speed, 2);
        Serial.print(" м/с (");
        Serial.print(speedKmh, 1);
        Serial.println(" км/ч)");
        
        // Дополнительная информация
        Serial.print("Сырых целей в кадре: ");
        Serial.println(rawTargets);
        
        Serial.print("Активных целей: ");
        Serial.println(radar.getActiveCount());
        
        Serial.println("==================");
        Serial.println();
    }
    
    // Небольшая задержка для стабильности (~20 Гц)
    delay(50);

if (radar.getActiveCount() > 0) {
        // Вся информация в одной строке
        Serial.printf(
            "ID:%d Dist:%.2fm Ang:%.1f° X:%.2f Y:%.2f Speed:%.2fm/s (%.1fkm/h) Raw:%d Act:%d\n",
            radar.getID(1),
            radar.getDistance(1),
            radar.getAngle(1),
            radar.getX(1),
            radar.getY(1),
            radar.getSpeed(1),
            radar.getSpeed(1) * 3.6f,
            radar.update(), // возвращает количество сырых целей
            radar.getActiveCount()
        );
    }

delay(1500);
}