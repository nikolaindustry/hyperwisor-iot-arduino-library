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


class HyperwisorIOT
{
public:
  HyperwisorIOT();
  void begin();
  void loop();

private:
  // WiFi & Real-time Communication
  nikolaindustryrealtime realtime;
  WebServer server;
  DNSServer dnsServer;
  HTTPClient http;


  // Core functions
  void setupMessageHandler();
  void performOTA(const char *otaUrl);
  void getcredentials();
  void startAPMode();
  void handle_provision();
  void connectToWiFi();
  String getSuccessHtml();
  String getErrorHtml(String errorMessage);

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
