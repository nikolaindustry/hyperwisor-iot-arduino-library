#ifndef HYPERWISOR_IOT_H
#define HYPERWISOR_IOT_H

#include <Arduino.h>
#include <WiFi.h>
#include <nikolaindustry-realtime.h>

class HyperwisorIOT {
public:
  HyperwisorIOT();
  void begin(const char *ssid, const char *password, const char *deviceId);
  void loop();

private:
  nikolaindustryrealtime realtime;
  void connectToWiFi(const char *ssid, const char *password);
  void setupMessageHandler();
};

#endif
