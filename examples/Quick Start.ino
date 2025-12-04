#include <Radar2450.h>

Radar2450 radar;

void setup() {
    Serial.begin(115200);
    radar.begin(16, 17);
    radar.setMaxDistance(6.0);
    radar.setSmoothingFactor(0.2);
}

void loop() {
    radar.update();
    int active = radar.getActiveTargetCount();
    if (active > 0) {
        float dist = radar.getTargetDistance(1);
        float angle = radar.getTargetAngle(1);
        float speed = radar.getTargetSpeed(1);
        Serial.printf("Closest: Dist=%.2fm, Angle=%.1f°, Speed=%.2fm/s\n", 
                     dist, angle, speed);
    }
    delay(50);
}