#ifndef RADAR2450_H
#define RADAR2450_H

#include <Arduino.h>
#include <LD2450.h>
#include <math.h>

class Radar2450 {
public:
    // ============ КОНСТРУКТОР ============
    Radar2450();
    
    // ============ ИНИЦИАЛИЗАЦИЯ ============
    // Инициализация с указанием пинов (ESP32)
    bool begin(int rx_pin, int tx_pin, bool debug = false);
    
    // Инициализация с готовым HardwareSerial
    bool begin(HardwareSerial& serial_port, bool debug = false);
    
    // ============ ОСНОВНЫЕ ФУНКЦИИ ============
    // Обновление данных радара
    int update();
    
    // Сброс всех целей
    void reset();
    
    // ============ КОНФИГУРАЦИЯ ============
    // Установка максимальной дистанции (метры)
    void setMaxDistance(float max_distance_m);
    
    // Установка коэффициента сглаживания (0.01-0.99)
    void setSmoothingFactor(float alpha);
    
    // Установка максимального количества целей для отслеживания (1-10)
    void setMaxTargets(int max_targets);
    
    // Установка ограничений по углу (градусы)
    void setAngleLimits(float min_angle, float max_angle);
    
    // ============ ПОЛУЧЕНИЕ ДАННЫХ ============
    // Количество активных целей
    int getActiveTargetCount();
    
    // Получение ID цели (цели отсортированы по расстоянию, 1 - ближайшая)
    int getTargetID(int target_index);
    
    // Дистанция до цели (метры)
    float getTargetDistance(int target_index);
    
    // Угол цели (градусы)
    float getTargetAngle(int target_index);
    
    // Координата X (метры)
    float getTargetX(int target_index);
    
    // Координата Y (метры)
    float getTargetY(int target_index);
    
    // Скорость цели (м/с)
    float getTargetSpeed(int target_index);
    
    // Скорость цели (км/ч)
    float getTargetSpeedKmh(int target_index);
    
    // Движется ли цель?
    bool getTargetMoving(int target_index);
    
    // ============ СЫРЫЕ ДАННЫЕ ============
    // Получение данных по индексу в массиве (0-9)
    int getRawTargetID(int array_index);
    float getRawTargetDistance(int array_index);
    float getRawTargetAngle(int array_index);
    float getRawTargetX(int array_index);
    float getRawTargetY(int array_index);
    float getRawTargetSpeed(int array_index);
    float getRawTargetSpeedKmh(int array_index);
    bool getRawTargetMoving(int array_index);
    
    // ============ ОТЛАДКА ============
    // Вывод отладочной информации
    void printDebugInfo(Stream& stream = Serial);
    
    // Получение частоты обновления (Гц)
    float getUpdateRateHz();
    
    // ============ ДОСТУП К БАЗОВОЙ БИБЛИОТЕКЕ ============
    // Прямой доступ к объекту LD2450 (для расширенного использования)
    LD2450* getRadar() { return &radar; }

private:
    // Структура данных цели
    struct RadarTargetData {
        int id = -1;                 // ID цели (-1 = пусто)
        float distance = 0;          // Дистанция (метры)
        float x = 0;                 // Координата X (метры)
        float y = 0;                 // Координата Y (метры)
        float angle = 0;             // Угол (градусы)
        float speed = 0;             // Скорость (м/с)
        bool moving = false;         // Движется ли цель
        unsigned long lastSeen = 0;  // Время последнего обнаружения (мс)
        int lostCounter = 0;         // Счетчик пропущенных кадров
        bool isActive = false;       // Активна ли цель
    };
    
    // Внутренние переменные
    LD2450 radar;
    HardwareSerial* radarSerial = nullptr;
    bool initialized = false;
    unsigned long lastUpdateTime = 0;
    float updateRateHz = 0;
    
    // Конфигурация
    float maxDistance = 8.0;
    float smoothingAlpha = 0.15;
    int maxTargets = 10;
    float minAngle = -60.0;
    float maxAngle = 60.0;
    
    // Массив целей
    RadarTargetData targets[10];
    int targetCounter = 0;
    
    // Внутренние методы
    void initializeTargets();
    int findTargetByProximity(float x, float y);
    int findFreeSlot();
    int assignTargetID();
    void updateTargetData(int slot, float x_m, float y_m, float speed_cm_s);
    void updateLostStatus();
    bool isWithinAngleLimits(float angle);
    void sortTargetsByDistance();
    int getSortedTargetIndex(int target_index);
    float calculateDistance(float x, float y);
    float calculateAngle(float x, float y);
};

#endif // RADAR2450_H