#include "Radar2450.h"

// Конструктор
Radar2450::Radar2450() {
    initializeTargets();
}

// Инициализация массива целей
void Radar2450::initializeTargets() {
    for (int i = 0; i < 10; i++) {
        targets[i].id = -1;
        targets[i].isActive = false;
        targets[i].lostCounter = 0;
    }
    targetCounter = 0;
}

// Инициализация с пинами
bool Radar2450::begin(int rx_pin, int tx_pin, bool debug) {
    #ifdef ESP32
    static HardwareSerial serial(1);
    serial.begin(256000, SERIAL_8N1, rx_pin, tx_pin);
    return begin(serial, debug);
    #else
    return false;
    #endif
}

// Инициализация с HardwareSerial
bool Radar2450::begin(HardwareSerial& serial_port, bool debug) {
    radarSerial = &serial_port;
    radar.begin(serial_port, false);
    
    if (!radar.waitForSensorMessage()) {
        initialized = true;
        return true;
    }
    return false;
}

// Обновление данных
int Radar2450::update() {
    if (!initialized) return 0;
    
    updateLostStatus();
    
    // Расчет частоты обновления
    unsigned long currentTime = millis();
    if (lastUpdateTime > 0) {
        float deltaTime = (currentTime - lastUpdateTime) / 1000.0;
        if (deltaTime > 0) {
            updateRateHz = 0.9 * updateRateHz + 0.1 * (1.0 / deltaTime);
        }
    }
    lastUpdateTime = currentTime;
    
    int detected = radar.read();
    if (detected <= 0) return 0;
    
    // Обработка целей
    for (int i = 0; i < radar.getSensorSupportedTargetCount(); i++) {
        LD2450::RadarTarget sensorTarget = radar.getTarget(i);
        if (!sensorTarget.valid) continue;
        
        float x_m = sensorTarget.x / 1000.0;
        float y_m = sensorTarget.y / 1000.0;
        float distance_m = sensorTarget.distance / 1000.0;
        float angle = calculateAngle(x_m, y_m);
        
        if (distance_m > maxDistance || distance_m < 0.1) continue;
        if (!isWithinAngleLimits(angle)) continue;
        
        int slot = findTargetByProximity(x_m, y_m);
        if (slot == -1) {
            slot = findFreeSlot();
            if (slot != -1) targets[slot].id = assignTargetID();
        }
        
        if (slot != -1) {
            updateTargetData(slot, x_m, y_m, sensorTarget.speed);
            targets[slot].lastSeen = currentTime;
            targets[slot].lostCounter = 0;
            targets[slot].isActive = true;
            targets[slot].moving = (abs(sensorTarget.speed) > 0);
        }
    }
    
    sortTargetsByDistance();
    return detected;
}

// Поиск цели по близости
int Radar2450::findTargetByProximity(float x, float y) {
    float minDistance = 0.5;
    for (int i = 0; i < maxTargets; i++) {
        if (targets[i].isActive) {
            float dx = x - targets[i].x;
            float dy = y - targets[i].y;
            float dist = calculateDistance(dx, dy);
            if (dist < minDistance) return i;
        }
    }
    return -1;
}

// Поиск свободного слота
int Radar2450::findFreeSlot() {
    for (int i = 0; i < maxTargets; i++) {
        if (targets[i].id == -1) return i;
    }
    for (int i = 0; i < maxTargets; i++) {
        if (!targets[i].isActive) return i;
    }
    return -1;
}

// Назначение ID цели
int Radar2450::assignTargetID() {
    targetCounter++;
    if (targetCounter > 9999) targetCounter = 1;
    return targetCounter;
}

// Обновление данных цели
void Radar2450::updateTargetData(int slot, float x_m, float y_m, float speed_cm_s) {
    targets[slot].x = smoothingAlpha * x_m + (1 - smoothingAlpha) * targets[slot].x;
    targets[slot].y = smoothingAlpha * y_m + (1 - smoothingAlpha) * targets[slot].y;
    targets[slot].distance = calculateDistance(targets[slot].x, targets[slot].y);
    targets[slot].angle = calculateAngle(targets[slot].x, targets[slot].y);
    targets[slot].speed = smoothingAlpha * (speed_cm_s / 100.0) + (1 - smoothingAlpha) * targets[slot].speed;
}

// Обновление статуса потерянных целей
void Radar2450::updateLostStatus() {
    unsigned long currentTime = millis();
    for (int i = 0; i < maxTargets; i++) {
        if (targets[i].id != -1) {
            targets[i].lostCounter++;
            if (currentTime - targets[i].lastSeen > 1000) targets[i].isActive = false;
            if (currentTime - targets[i].lastSeen > 3000) targets[i].id = -1;
        }
    }
}

// Проверка угловых ограничений
bool Radar2450::isWithinAngleLimits(float angle) {
    return (angle >= minAngle && angle <= maxAngle);
}

// Сортировка целей по расстоянию
void Radar2450::sortTargetsByDistance() {
    for (int i = 0; i < maxTargets - 1; i++) {
        for (int j = i + 1; j < maxTargets; j++) {
            bool iActive = targets[i].isActive && targets[i].id != -1;
            bool jActive = targets[j].isActive && targets[j].id != -1;
            
            if ((!iActive && jActive) || 
                (iActive && jActive && targets[j].distance < targets[i].distance)) {
                RadarTargetData temp = targets[i];
                targets[i] = targets[j];
                targets[j] = temp;
            }
        }
    }
}

// Получение индекса отсортированной цели
int Radar2450::getSortedTargetIndex(int target_index) {
    if (target_index < 1) return -1;
    int count = 0;
    for (int i = 0; i < maxTargets; i++) {
        if (targets[i].isActive && targets[i].id != -1) {
            count++;
            if (count == target_index) return i;
        }
    }
    return -1;
}

// Расчет дистанции
float Radar2450::calculateDistance(float x, float y) {
    return sqrt(x * x + y * y);
}

// Расчет угла
float Radar2450::calculateAngle(float x, float y) {
    return atan2(x, y) * 180.0 / PI;
}

// ============ ПУБЛИЧНЫЕ МЕТОДЫ ============

// Конфигурация
void Radar2450::setMaxDistance(float max_distance_m) {
    maxDistance = max_distance_m;
}

void Radar2450::setSmoothingFactor(float alpha) {
    smoothingAlpha = constrain(alpha, 0.01, 0.99);
}

void Radar2450::setMaxTargets(int max_targets) {
    maxTargets = constrain(max_targets, 1, 10);
}

void Radar2450::setAngleLimits(float min_angle, float max_angle) {
    minAngle = min_angle;
    maxAngle = max_angle;
}

void Radar2450::reset() {
    initializeTargets();
}

// Получение данных
int Radar2450::getActiveTargetCount() {
    int count = 0;
    for (int i = 0; i < maxTargets; i++) {
        if (targets[i].isActive && targets[i].id != -1) count++;
    }
    return count;
}

int Radar2450::getTargetID(int target_index) {
    int idx = getSortedTargetIndex(target_index);
    return (idx != -1) ? targets[idx].id : -1;
}

float Radar2450::getTargetDistance(int target_index) {
    int idx = getSortedTargetIndex(target_index);
    return (idx != -1) ? targets[idx].distance : 0;
}

float Radar2450::getTargetAngle(int target_index) {
    int idx = getSortedTargetIndex(target_index);
    return (idx != -1) ? targets[idx].angle : 0;
}

float Radar2450::getTargetX(int target_index) {
    int idx = getSortedTargetIndex(target_index);
    return (idx != -1) ? targets[idx].x : 0;
}

float Radar2450::getTargetY(int target_index) {
    int idx = getSortedTargetIndex(target_index);
    return (idx != -1) ? targets[idx].y : 0;
}

float Radar2450::getTargetSpeed(int target_index) {
    int idx = getSortedTargetIndex(target_index);
    return (idx != -1) ? targets[idx].speed : 0;
}

float Radar2450::getTargetSpeedKmh(int target_index) {
    return getTargetSpeed(target_index) * 3.6;
}

bool Radar2450::getTargetMoving(int target_index) {
    int idx = getSortedTargetIndex(target_index);
    return (idx != -1) ? targets[idx].moving : false;
}

// Сырые данные
int Radar2450::getRawTargetID(int array_index) {
    if (array_index < 0 || array_index >= maxTargets) return -1;
    return targets[array_index].id;
}

float Radar2450::getRawTargetDistance(int array_index) {
    if (array_index < 0 || array_index >= maxTargets) return 0;
    return targets[array_index].distance;
}

float Radar2450::getRawTargetAngle(int array_index) {
    if (array_index < 0 || array_index >= maxTargets) return 0;
    return targets[array_index].angle;
}

float Radar2450::getRawTargetX(int array_index) {
    if (array_index < 0 || array_index >= maxTargets) return 0;
    return targets[array_index].x;
}

float Radar2450::getRawTargetY(int array_index) {
    if (array_index < 0 || array_index >= maxTargets) return 0;
    return targets[array_index].y;
}

float Radar2450::getRawTargetSpeed(int array_index) {
    if (array_index < 0 || array_index >= maxTargets) return 0;
    return targets[array_index].speed;
}

float Radar2450::getRawTargetSpeedKmh(int array_index) {
    return getRawTargetSpeed(array_index) * 3.6;
}

bool Radar2450::getRawTargetMoving(int array_index) {
    if (array_index < 0 || array_index >= maxTargets) return false;
    return targets[array_index].moving;
}

// Отладка
void Radar2450::printDebugInfo(Stream& stream) {
    stream.printf("Radar2450 - Update Rate: %.1f Hz\n", updateRateHz);
    stream.printf("Active Targets: %d/%d\n", getActiveTargetCount(), maxTargets);
    for (int i = 0; i < maxTargets; i++) {
        if (targets[i].id != -1) {
            stream.printf("  Slot %d: ID=%d, Dist=%.2fm, Angle=%.1f°, Pos=(%.2f,%.2f)m, Speed=%.2fm/s\n",
                         i, targets[i].id, targets[i].distance, targets[i].angle,
                         targets[i].x, targets[i].y, targets[i].speed);
        }
    }
}

float Radar2450::getUpdateRateHz() {
    return updateRateHz;
}