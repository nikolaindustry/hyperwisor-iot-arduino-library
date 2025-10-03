#include <hyperwisor-iot.h>

// Initialize the Hyperwisor IoT library
HyperwisorIOT hw;

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("Hyperwisor IoT Database API Test Sketch");
  Serial.println("========================================");
  
  // Set your API keys
  hw.setApiKeys("your-api-key", "your-secret-key");
  
  // Initialize the library
  hw.begin();
  
  Serial.println("Library initialized. Uncomment the function you want to test below.");
  Serial.println();
}

void loop() {
  // Test functions - Uncomment one at a time to test
  
  // 1. Send data to database
  // testSendDatabaseData();
  
  // 2. Get data from database
  // testGetDatabaseData();
  
  // 3. Update data in database
  // testUpdateDatabaseData();
  
  // 4. Delete data from database
  // testDeleteDatabaseData();
  
  // 5. Onboard a new device
  // testOnboardDevice();
  
  // Run the library's loop function
  hw.loop();
  
  // Add a delay to avoid overwhelming the serial monitor
  delay(5000);
}

// Function to test sending data to the database
void testSendDatabaseData() {
  Serial.println("Testing sendDatabaseData...");
  
  hw.sendDatabaseData("product-uuid", "device-uuid", "temperature_readings", [](JsonObject &data) {
    data["temperature"] = 24.5;
    data["humidity"] = 65.2;
    data["timestamp"] = millis();
  });
  
  Serial.println("sendDatabaseData test completed.");
  Serial.println();
}

// Function to test sending data to the database with response
void testSendDatabaseDataWithResponse() {
  Serial.println("Testing sendDatabaseDataWithResponse...");
  
  DynamicJsonDocument response = hw.sendDatabaseDataWithResponse("product-uuid", "device-uuid", "temperature_readings", [](JsonObject &data) {
    data["temperature"] = 24.5;
    data["humidity"] = 65.2;
    data["timestamp"] = millis();
  });
  
  if (response["success"]) {
    Serial.println("Data sent successfully!");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    
    if (response.containsKey("data")) {
      Serial.println("Response data:");
      serializeJson(response["data"], Serial);
      Serial.println();
    }
  } else {
    Serial.println("Error sending data:");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    Serial.print("Error: ");
    Serial.println(response["error"].as<String>());
  }
  
  Serial.println("sendDatabaseDataWithResponse test completed.");
  Serial.println();
}

// Function to test getting data from the database
void testGetDatabaseData() {
  Serial.println("Testing getDatabaseData...");
  
  hw.getDatabaseData("product-uuid", "temperature_readings", 10);
  
  Serial.println("getDatabaseData test completed.");
  Serial.println();
}

// Function to test getting data from the database with response
void testGetDatabaseDataWithResponse() {
  Serial.println("Testing getDatabaseDataWithResponse...");
  
  DynamicJsonDocument response = hw.getDatabaseDataWithResponse("product-uuid", "temperature_readings", 10);
  
  if (response["success"]) {
    Serial.println("Data retrieved successfully!");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    
    if (response.containsKey("data")) {
      Serial.println("Retrieved data:");
      serializeJson(response["data"], Serial);
      Serial.println();
    }
  } else {
    Serial.println("Error retrieving data:");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    Serial.print("Error: ");
    Serial.println(response["error"].as<String>());
  }
  
  Serial.println("getDatabaseDataWithResponse test completed.");
  Serial.println();
}

// Function to test updating data in the database
void testUpdateDatabaseData() {
  Serial.println("Testing updateDatabaseData...");
  
  // Replace "data-uuid" with the actual data ID you want to update
  hw.updateDatabaseData("data-uuid", [](JsonObject &data) {
    data["temperature"] = 25.1;
    data["humidity"] = 67.8;
  });
  
  Serial.println("updateDatabaseData test completed.");
  Serial.println();
}

// Function to test updating data in the database with response
void testUpdateDatabaseDataWithResponse() {
  Serial.println("Testing updateDatabaseDataWithResponse...");
  
  // Replace "data-uuid" with the actual data ID you want to update
  DynamicJsonDocument response = hw.updateDatabaseDataWithResponse("data-uuid", [](JsonObject &data) {
    data["temperature"] = 25.1;
    data["humidity"] = 67.8;
  });
  
  if (response["success"]) {
    Serial.println("Data updated successfully!");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    
    if (response.containsKey("data")) {
      Serial.println("Response data:");
      serializeJson(response["data"], Serial);
      Serial.println();
    }
  } else {
    Serial.println("Error updating data:");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    Serial.print("Error: ");
    Serial.println(response["error"].as<String>());
  }
  
  Serial.println("updateDatabaseDataWithResponse test completed.");
  Serial.println();
}

// Function to test deleting data from the database
void testDeleteDatabaseData() {
  Serial.println("Testing deleteDatabaseData...");
  
  // Replace "data-uuid" with the actual data ID you want to delete
  hw.deleteDatabaseData("data-uuid");
  
  Serial.println("deleteDatabaseData test completed.");
  Serial.println();
}

// Function to test deleting data from the database with response
void testDeleteDatabaseDataWithResponse() {
  Serial.println("Testing deleteDatabaseDataWithResponse...");
  
  // Replace "data-uuid" with the actual data ID you want to delete
  DynamicJsonDocument response = hw.deleteDatabaseDataWithResponse("data-uuid");
  
  if (response["success"]) {
    Serial.println("Data deleted successfully!");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    
    if (response.containsKey("data")) {
      Serial.println("Response data:");
      serializeJson(response["data"], Serial);
      Serial.println();
    }
  } else {
    Serial.println("Error deleting data:");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    Serial.print("Error: ");
    Serial.println(response["error"].as<String>());
  }
  
  Serial.println("deleteDatabaseDataWithResponse test completed.");
  Serial.println();
}

// Function to test onboarding a new device
void testOnboardDevice() {
  Serial.println("Testing onboardDevice...");
  
  hw.onboardDevice("product-uuid", "user-uuid", "Kitchen Smart Light", "SN123456");
  
  Serial.println("onboardDevice test completed.");
  Serial.println();
}

// Function to test onboarding a new device with response
void testOnboardDeviceWithResponse() {
  Serial.println("Testing onboardDeviceWithResponse...");
  
  DynamicJsonDocument response = hw.onboardDeviceWithResponse("product-uuid", "user-uuid", "Kitchen Smart Light", "SN123456");
  
  if (response["success"]) {
    Serial.println("Device onboarded successfully!");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    
    if (response.containsKey("data")) {
      Serial.println("Response data:");
      serializeJson(response["data"], Serial);
      Serial.println();
    }
  } else {
    Serial.println("Error onboarding device:");
    Serial.print("HTTP Response Code: ");
    Serial.println(response["http_response_code"].as<int>());
    Serial.print("Error: ");
    Serial.println(response["error"].as<String>());
  }
  
  Serial.println("onboardDeviceWithResponse test completed.");
  Serial.println();
}