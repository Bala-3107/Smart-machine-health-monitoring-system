
#include <Arduino.h>
#include "DHT.h"

// ==================================
// PIN CONFIGURATION
// ==================================

#define DHT_PIN       4
#define DHT_TYPE      DHT22

#define CURRENT_PIN   34
#define VIBRATION_PIN 27

#define LED_PIN       26
#define BUZZER_PIN    25

// ==================================
// SENSOR CONFIGURATION
// ==================================

DHT dht(DHT_PIN, DHT_TYPE);

const float TEMP_LIMIT = 40.0;
const float HUMIDITY_MIN = 30.0;
const float HUMIDITY_MAX = 70.0;
const float CURRENT_LIMIT = 3.5;

// ==================================
// DATA STRUCTURES
// ==================================

struct SensorData {
  float temperature;
  float humidity;
  float current;
  int adcValue;
  bool vibrationDetected;
  bool dhtError;
};

struct AlarmData {
  bool abnormal;
};

// ==================================
// FREERTOS QUEUES
// ==================================

QueueHandle_t sensorQueue;
QueueHandle_t alarmQueue;

// ==================================
// TASK 1: SENSOR READING
// ==================================

void SensorTask(void *parameter) {

  SensorData data;

  for (;;) {

    // Read DHT22
    data.temperature = dht.readTemperature();
    data.humidity = dht.readHumidity();

    data.dhtError =
      isnan(data.temperature) || isnan(data.humidity);

    // Read simulated current
    data.adcValue = analogRead(CURRENT_PIN);
    data.current = (data.adcValue / 4095.0) * 5.0;

    // Read vibration button
    int vibrationValue = digitalRead(VIBRATION_PIN);
    data.vibrationDetected = (vibrationValue == LOW);

    // Send latest sensor data to queue
    xQueueOverwrite(sensorQueue, &data);

    Serial.println("\n[SENSOR TASK] Sensor readings updated");

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

// ==================================
// TASK 2: MACHINE MONITORING
// ==================================

void MonitoringTask(void *parameter) {

  SensorData data;
  AlarmData alarm;

  for (;;) {

    // Get latest sensor data
    if (xQueuePeek(sensorQueue, &data, portMAX_DELAY) == pdPASS) {

      alarm.abnormal = false;

      Serial.println("\n========== MACHINE DATA ==========");

      // Temperature and humidity
      if (!data.dhtError) {

        Serial.print("Temperature : ");
        Serial.print(data.temperature);
        Serial.println(" C");

        Serial.print("Humidity    : ");
        Serial.print(data.humidity);
        Serial.println(" %");

        if (data.temperature > TEMP_LIMIT) {
          Serial.println("Temperature Status: HIGH");
          alarm.abnormal = true;
        } else {
          Serial.println("Temperature Status: NORMAL");
        }

        if (data.humidity < HUMIDITY_MIN) {
          Serial.println("Humidity Status: TOO LOW");
          alarm.abnormal = true;
        }
        else if (data.humidity > HUMIDITY_MAX) {
          Serial.println("Humidity Status: TOO HIGH");
          alarm.abnormal = true;
        }
        else {
          Serial.println("Humidity Status: NORMAL");
        }

      } else {

        Serial.println("DHT22 Sensor Reading Error!");

      }

      // Current monitoring
      Serial.print("ADC Value : ");
      Serial.println(data.adcValue);

      Serial.print("Current   : ");
      Serial.print(data.current, 2);
      Serial.println(" A (simulated)");

      if (data.current > CURRENT_LIMIT) {
        Serial.println("Current Status: HIGH");
        alarm.abnormal = true;
      } else {
        Serial.println("Current Status: NORMAL");
      }

      // Vibration monitoring
      if (data.vibrationDetected) {
        Serial.println("Vibration: DETECTED");
        alarm.abnormal = true;
      } else {
        Serial.println("Vibration: NORMAL");
      }

      // Send machine condition to alarm task
      xQueueOverwrite(alarmQueue, &alarm);

      Serial.println("==================================");
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

// ==================================
// TASK 3: ALARM CONTROL
// ==================================

void AlarmTask(void *parameter) {

  AlarmData alarm;

  for (;;) {

    // Wait for machine condition
    if (xQueueReceive(alarmQueue, &alarm, portMAX_DELAY) == pdPASS) {

      if (alarm.abnormal) {

        digitalWrite(LED_PIN, HIGH);
        tone(BUZZER_PIN, 2000);

        Serial.println("[ALARM TASK] WARNING!");
        Serial.println("[ALARM TASK] ABNORMAL MACHINE CONDITION");

      } else {

        digitalWrite(LED_PIN, LOW);
        noTone(BUZZER_PIN);

        Serial.println("[ALARM TASK] MACHINE STATUS: NORMAL");

      }
    }
  }
}

// ==================================
// SETUP
// ==================================

void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(CURRENT_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("\n================================");
  Serial.println(" INDUSTRIAL TELEMETRY NODE");
  Serial.println(" ESP32 FREERTOS INITIALIZATION");
  Serial.println("================================");

  // Create queues
  sensorQueue = xQueueCreate(1, sizeof(SensorData));
  alarmQueue = xQueueCreate(1, sizeof(AlarmData));

  if (sensorQueue == NULL || alarmQueue == NULL) {
    Serial.println("ERROR: Queue creation failed!");
    while (true) {
      delay(1000);
    }
  }

  // Create FreeRTOS tasks

  xTaskCreatePinnedToCore(
    SensorTask,
    "Sensor Task",
    4096,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    MonitoringTask,
    "Monitoring Task",
    4096,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    AlarmTask,
    "Alarm Task",
    4096,
    NULL,
    2,
    NULL,
    1
  );

  Serial.println("FreeRTOS tasks created successfully!");
}

// ==================================
// MAIN LOOP
// ==================================

void loop() {

  // FreeRTOS tasks perform the main operations.
  // Keep the Arduino loop task idle.

  vTaskDelay(pdMS_TO_TICKS(1000));
}
