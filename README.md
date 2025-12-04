# Radar2450 Library

Расширенная обертка для библиотеки HLK-LD2450 с фильтрацией, трекингом целей и сглаживанием данных.

![Radar2450](https://img.shields.io/badge/version-1.0.0-blue)
![Platform](https://img.shields.io/badge/platform-ESP32%20%7C%20ESP8266-green)
![License](https://img.shields.io/badge/license-MIT-yellow)

## ОСНОВНЫЕ ВОЗМОЖНОСТИ

- Экспоненциальное сглаживание координат целей
- Устойчивый трекинг целей с уникальными ID
- Расчет скорости в м/с и км/ч
- Настраиваемые ограничения по углу и дистанции
- Сортировка целей по расстоянию (ближайшие первые)
- Конфигурируемый коэффициент сглаживания
- Отладочная информация и статистика
- Совместимость с ESP32 и ESP8266

## ТРЕБОВАНИЯ К ОБОРУДОВАНИЮ

Библиотека `Radar2450` является расширением для библиотеки `HLK-LD2450` и требует ее установки. Датчик `HLK-LD2450` работает на частоте 24 ГГц и использует последовательное соединение на скорости 256000 бод. Рекомендуется использовать микроконтроллеры с аппаратным UART для стабильной работы.

## API

### Инициализация

```cpp
void begin(int rx_pin, int tx_pin, long baud_rate = 256000);
```cpp
### Конфигурация

```cpp
void setSmoothingFactor(float factor);
void setMaxDistance(float distance);
void setMinDistance(float distance);
void setAngleFilter(float min_angle, float max_angle);

Методы для настройки параметров фильтрации и обработки данных.

### Получение данных

```cpp

int getActiveTargetCount();
float getTargetDistance(int index);
float getTargetAngle(int index);
float getTargetX(int index);
float getTargetY(int index);
float getTargetSpeedMs(int index);
float getTargetSpeedKmh(int index);
int getTargetID(int index);



