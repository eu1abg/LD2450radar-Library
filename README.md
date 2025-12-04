# Radar2450 Library

Advanced wrapper library for HLK-LD2450 radar sensors with filtering, tracking, and smoothing.

## Features
- ✅ Exponential smoothing of target coordinates
- ✅ Persistent target tracking with unique IDs
- ✅ Velocity calculation (m/s and km/h)
- ✅ Configurable angle limits and maximum distance
- ✅ Target sorting by distance
- ✅ Configurable smoothing factor
- ✅ Debug information output
- ✅ Compatible with ESP32 and ESP8266

## Installation

### Arduino IDE
1. Install [HLK-LD2450](https://github.com/RBEGamer/HLK-LD2450) library via Library Manager
2. Download Radar2450 as ZIP
3. Sketch → Include Library → Add .ZIP Library

### PlatformIO
Add to `platformio.ini`:
```ini
lib_deps = 
    Radar2450
    HLK-LD2450