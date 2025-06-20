#ifndef HYPERWISOR_IOT_H
#define HYPERWISOR_IOT_H

#include <Arduino.h>
#include <WiFi.h>
#include <nikolaindustry-realtime.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Update.h>
#include <DNSServer.h>
#include <HTTPClient.h>
#include "HyperTaskManager.h"

typedef std::function<void(JsonObject &msg)> UserCommandCallback;

class HyperwisorIOT
{
public:
  HyperwisorIOT();
  void begin();
  void loop();

  // User-defined command callback
  void setUserCommandHandler(UserCommandCallback cb);
  void sendTo(const String &targetId, std::function<void(JsonObject &)> payloadBuilder);
  HyperTaskManager &getTaskManager();
  void saveGPIOState(int pin, int state);
  int loadGPIOState(int pin);
  void restoreAllGPIOStates();
  String getDeviceId();

private:
  // WiFi & Real-time Communication
  nikolaindustryrealtime realtime;
  WebServer server;
  DNSServer dnsServer;
  HTTPClient http;
  HyperTaskManager taskManager;

  // Core functions
  void setupMessageHandler();
  void performOTA(const char *otaUrl);
  void getcredentials();
  void startAPMode();
  void handle_provision();
  void connectToWiFi();

  String getSuccessHtml();
  String getErrorHtml(String errorMessage);
  UserCommandCallback userCommandCallback = nullptr;

  // Credentials and config
  String ssid, password, userid, deviceid, productid, email, loaclip, macid, newtarget, versionid;
  const char *apSSID = "NIKOLAINDUSTRY_Setup";
  const char *apPassword = "0123456789";
  String fversion = "0.0.1";

  // Retry Logic
  unsigned long lastReconnectAttempt = 0;
  const unsigned long reconnectInterval = 10000;
  int retryCount = 0;
  const int maxRetries = 6;
};

#endif
