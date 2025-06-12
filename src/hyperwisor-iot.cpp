#include "hyperwisor-iot.h"

HyperwisorIOT::HyperwisorIOT() {}

void HyperwisorIOT::begin(const char *ssid, const char *password, const char *deviceId) {
  connectToWiFi(ssid, password);
  realtime.begin(deviceId); // Start WebSocket after Wi-Fi
  setupMessageHandler();
}

void HyperwisorIOT::connectToWiFi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  Serial.print("🔌 Connecting to Wi-Fi");

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi connected");
    Serial.print("📡 IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Wi-Fi connection failed");
  }
}

void HyperwisorIOT::setupMessageHandler() {
  realtime.setOnMessageCallback([](JsonObject &obj) {
    Serial.println("📨 Received JSON:");
    serializeJsonPretty(obj, Serial);
    Serial.println();

    // TODO: Add your command parsing and logic here
  });
}

void HyperwisorIOT::loop() {
  realtime.loop(); // Required to keep WebSocket alive
}
