/**
 * FSR Pressure Sensor Example
 * 
 * This example demonstrates how to send 7-point FSR matrix sensor data
 * to Hyperwisor IoT dashboard widgets.
 * 
 * Features:
 * - Individual widget updates for each sensor
 * - Optional: Chart widget with all sensor values
 * - Optional: Data logging for historical visualization
 */

#include <hyperwisor-iot.h>

HyperwisorIOT device;

// Define the pins for the 7 sensors
const int fsrPins[] = {32, 33, 34, 35, 36, 39, 4}; 
const int numSensors = 7;

int sensorReadings[numSensors];

// Replace with your actual target and widget IDs
String targetId = "your-dashboard-id";

// Individual widget IDs for each sensor (create these in your dashboard)
String sensorWidgetIds[] = {
  "pressure-sensor-1",
  "pressure-sensor-2", 
  "pressure-sensor-3",
  "pressure-sensor-4",
  "pressure-sensor-5",
  "pressure-sensor-6",
  "pressure-sensor-7"
};

// Optional: Chart widget to display all sensors
String chartWidgetId = "pressure-chart";

// Optional: Data logger config ID
String configId = "fsr-sensor-config";

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 500; // Update every 500ms

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== FSR Pressure Sensor Example ===\n");
  
  // Initialize FSR pins
  for (int i = 0; i < numSensors; i++) {
    pinMode(fsrPins[i], INPUT);
  }
  Serial.println("FSR sensors initialized on pins: 32, 33, 34, 35, 36, 39, 4");
  
  // Initialize Hyperwisor IoT
  device.begin();
  
  Serial.println("Device initialized!");
  Serial.println("Sending pressure data to dashboard...");
}

void loop() {
  device.loop();
  
  // Read all sensors
  readAllSensors();
  
  // Send updates at regular intervals
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();
    
    // Method 1: Update individual widgets for each sensor
    updateIndividualWidgets();
    
    // Method 2: Update chart widget with all sensor values
    // updateChartWidget();
    
    // Method 3: Log data for historical visualization
    // logSensorData();
    
    // Print to Serial for debugging
    printSensorData();
  }
}

void readAllSensors() {
  for (int i = 0; i < numSensors; i++) {
    sensorReadings[i] = analogRead(fsrPins[i]);
  }
}

void updateIndividualWidgets() {
  for (int i = 0; i < numSensors; i++) {
    // Send each sensor value to its corresponding widget
    device.updateWidget(targetId, sensorWidgetIds[i], sensorReadings[i]);
  }
  Serial.println("Updated individual widgets");
}

void updateChartWidget() {
  // Convert sensor readings to float vector for chart widget
  std::vector<float> chartData;
  for (int i = 0; i < numSensors; i++) {
    chartData.push_back((float)sensorReadings[i]);
  }
  
  device.updateWidget(targetId, chartWidgetId, chartData);
  Serial.println("Updated chart widget");
}

void logSensorData() {
  // Send structured data for logging and historical charts
  device.send_Sensor_Data_logger(
    targetId,
    configId,
    {
      {"sensor1", (float)sensorReadings[0]},
      {"sensor2", (float)sensorReadings[1]},
      {"sensor3", (float)sensorReadings[2]},
      {"sensor4", (float)sensorReadings[3]},
      {"sensor5", (float)sensorReadings[4]},
      {"sensor6", (float)sensorReadings[5]},
      {"sensor7", (float)sensorReadings[6]}
    }
  );
  Serial.println("Logged sensor data");
}

void printSensorData() {
  Serial.print("Pressure Data: ");
  for (int i = 0; i < numSensors; i++) {
    Serial.print("S");
    Serial.print(i + 1);
    Serial.print("=");
    Serial.print(sensorReadings[i]);
    if (i < numSensors - 1) {
      Serial.print(" | ");
    }
  }
  Serial.println();
}
