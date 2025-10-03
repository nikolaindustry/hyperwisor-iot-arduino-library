#include <hyperwisor-iot.h>

// Initialize the Hyperwisor IoT library
HyperwisorIOT hw;

// Timer for testing time functions
unsigned long lastTimeCheck = 0;
const unsigned long timeCheckInterval = 30000; // 30 seconds

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("Hyperwisor IoT Time and Date API Test Sketch");
  Serial.println("===========================================");
  
  // Set timezone to IST (UTC+5:30) - adjust as needed for your location
  // Supported timezones: "IST", "EST", "PST", "UTC"
  hw.setTimezone("IST");
  
  // Initialize the library
  hw.begin();
  
  Serial.println("Library initialized with IST timezone. Testing time and date functions every 30 seconds...");
  Serial.println();
}

void loop() {
  // Test time and date functions every 30 seconds
  if (millis() - lastTimeCheck >= timeCheckInterval) {
    testNetworkTimeAndDate();
    lastTimeCheck = millis();
  }
  
  // Run the library's loop function
  hw.loop();
}

void testNetworkTimeAndDate() {
  Serial.println("Testing network time and date functions (using NTP with timezone)...");
  
  // Get network time
  String time = hw.getNetworkTime();
  if (time.length() > 0) {
    Serial.print("Current network time: ");
    Serial.println(time);
  } else {
    Serial.println("Failed to get network time");
  }
  
  // Get network date
  String date = hw.getNetworkDate();
  if (date.length() > 0) {
    Serial.print("Current network date: ");
    Serial.println(date);
  } else {
    Serial.println("Failed to get network date");
  }
  
  // Get network date and time
  String datetime = hw.getNetworkDateTime();
  if (datetime.length() > 0) {
    Serial.print("Current network date and time: ");
    Serial.println(datetime);
  } else {
    Serial.println("Failed to get network date and time");
  }
  
  Serial.println();
}