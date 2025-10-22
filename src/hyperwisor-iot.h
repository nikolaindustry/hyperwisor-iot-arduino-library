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
#include <ArduinoJson.h>
#include <time.h>
#include <sys/time.h>

typedef std::function<void(JsonObject &msg)> UserCommandCallback;

class HyperwisorIOT
{
public:

  // Constructor
  HyperwisorIOT();

  // Core functions
  void begin();
  void loop();

  // API key and secret key
  void setApiKeys(const String &apiKey, const String &secretKey);

  // User-defined command callback
  void setUserCommandHandler(UserCommandCallback cb);
  void sendTo(const String &targetId, std::function<void(JsonObject &)> payloadBuilder);

  // GPIO functions
  void saveGPIOState(int pin, int state);
  int loadGPIOState(int pin);
  void restoreAllGPIOStates();

  // Device info
  String getDeviceId();
  String getUserId();


  // Data logger
  void send_Sensor_Data_logger(const String &targetId, const String &configId, std::initializer_list<std::pair<const char *, float>> dataList);

  // Widget functions
  void updateWidget(const String &targetId, const String &widgetId, const String &value);
  void updateWidget(const String &targetId, const String &widgetId, float value);
  
  // Dialog functions
  void showDialog(const String &targetId, const String &title, const String &description, const String &icon = "info"); // icon options: info (default), warning, success, error, risk

  // Device status
  void sendDeviceStatus(const String &targetId);
  
  // Time and date functions
  void initNTP();
  void setTimezone(const char* timezone);
  String getNetworkTime();
  String getNetworkDate();
  String getNetworkDateTime();
  
  // Database functions
  void insertDatainDatabase(const String &productId, const String &deviceId, const String &tableName, std::function<void(JsonObject &)> dataBuilder);
  DynamicJsonDocument insertDatainDatabaseWithResponse(const String &productId, const String &deviceId, const String &tableName, std::function<void(JsonObject &)> dataBuilder);
  void getDatabaseData(const String &productId, const String &tableName, int limit = 50);
  DynamicJsonDocument getDatabaseDataWithResponse(const String &productId, const String &tableName, int limit = 50);
  void updateDatabaseData(const String &dataId, std::function<void(JsonObject &)> dataBuilder);
  DynamicJsonDocument updateDatabaseDataWithResponse(const String &dataId, std::function<void(JsonObject &)> dataBuilder);
  void deleteDatabaseData(const String &dataId);
  DynamicJsonDocument deleteDatabaseDataWithResponse(const String &dataId);
  
  // Device onboarding functions
  void onboardDevice(const String &productId, const String &userId, const String &deviceName, const String &deviceIdentifier);
  DynamicJsonDocument onboardDeviceWithResponse(const String &productId, const String &userId, const String &deviceName, const String &deviceIdentifier);

  // JSON response versions
  void sendSMS(const String &productId, const String &to, const String &message);
  DynamicJsonDocument sendSMSWithResponse(const String &productId, const String &to, const String &message);

  // Authentication
  void authenticateUser(const String &email, const String &password);
  DynamicJsonDocument authenticateUserWithResponse(const String &email, const String &password);

  // Task manager
  HyperTaskManager &getTaskManager();

  // JSON utility functions
  JsonObject findCommand(JsonObject& payload, const char* commandName);
  JsonObject findAction(JsonObject& payload, const char* commandName, const char* actionName);
  JsonObject findParams(JsonObject& payload, const char* commandName, const char* actionName);

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



  // User-defined command callback
  UserCommandCallback userCommandCallback = nullptr;

  // Credentials and config
  String ssid, password, userid, deviceid, productid, email, loaclip, macid, newtarget, versionid;
  String apiKey, secretKey;

  // AP Mode
  const char *apSSID = "NIKOLAINDUSTRY_Setup";
  const char *apPassword = "0123456789";
  String fversion = "0.0.1";

  // Retry Logic
  unsigned long lastReconnectAttempt = 0;
  const unsigned long reconnectInterval = 10000;
  int retryCount = 0;
  const int maxRetries = 6;
  
  // NTP
  bool ntpInitialized = false;
  String timezone = "UTC0";  // Default to UTC
};

#endif
