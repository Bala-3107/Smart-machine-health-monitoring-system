# Industrial Telemetry Node using ESP32 and FreeRTOS

## Project Overview
An IoT-based industrial machine health monitoring system developed using ESP32 and FreeRTOS. The system monitors machine parameters and detects abnormal operating conditions.

## Technologies Used
- ESP32
- FreeRTOS
- Embedded C / Arduino C++
- DHT22 Temperature and Humidity Sensor
- Potentiometer (Simulated Current Sensor)
- Pushbutton (Simulated Vibration Sensor)
- Wi-Fi
- ThingSpeak IoT Cloud
- Wokwi Simulator

## Features
- Temperature monitoring
- Humidity monitoring
- Simulated current monitoring
- Vibration detection
- Real-time abnormal condition detection
- LED and buzzer alarm system
- FreeRTOS multitasking
- Cloud data monitoring using ThingSpeak

## FreeRTOS Tasks
1. Sensor Task – Reads sensor values.
2. Monitoring Task – Checks sensor values against thresholds.
3. Alarm Task – Controls LED and buzzer.
4. Cloud Task – Sends sensor data to ThingSpeak.

## Pin Configuration

| Component | ESP32 GPIO |
|---|---|
| DHT22 | GPIO 4 |
| Current Simulation | GPIO 34 |
| Vibration Button | GPIO 27 |
| LED | GPIO 26 |
| Buzzer | GPIO 25 |

## Simulation Environment
The project is developed and simulated using Wokwi.

## Future Improvements
- Integration of a real vibration sensor
- Physical ESP32 hardware implementation
- Advanced IoT dashboard
- Historical data analysis and machine health prediction

## Project Status
Simulation-based prototype.