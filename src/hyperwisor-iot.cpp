#include "hyperwisor-iot.h"

Preferences preferences;
Preferences gpioPreferences;
bool provision_request = false;

HyperwisorIOT::HyperwisorIOT()
{
}

void HyperwisorIOT::begin()
{
  getcredentials();

  if (!ssid.isEmpty() && !password.isEmpty())
  {
    connectToWiFi();
  }
  else
  {
    startAPMode();
  }
  taskManager.begin();
}

void HyperwisorIOT::loop()
{
  realtime.loop();
  taskManager.loop();
  if (WiFi.getMode() == WIFI_AP)
  {
    dnsServer.processNextRequest();
    server.handleClient();
  }
}

void HyperwisorIOT::startAPMode()
{
  Serial.println("Starting in AP mode...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);
  Serial.print("AP Started. IP: ");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/api/provision", [this]()
            { handle_provision(); });
  server.begin();
  Serial.println("HTTP server started.");
}

void HyperwisorIOT::handle_provision()
{
  if (server.hasArg("ssid") && server.arg("ssid").length() > 0)
  {
    ssid = server.arg("ssid");
    password = server.arg("password");
    deviceid = server.arg("target_id");

    preferences.begin("wifi-creds", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.putString("deviceid", deviceid);
    preferences.end();

    server.send(200, "text/html", getSuccessHtml());
    provision_request = true;
    delay(2000);
    ESP.restart();
  }
  else
  {
    server.send(400, "text/html", getErrorHtml("Missing_SSID"));
  }
}

String HyperwisorIOT::getSuccessHtml()
{
  String message = "Device_is_connecting_to_the_new_network.";
  String redirectUrl = "hypervisorv4://provisioning?status=success&message=" + message;

  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Provisioning Success</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f0f2f5; text-align: center; padding: 20px; }
    .card { background: white; padding: 40px; border-radius: 12px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); max-width: 400px; }
    h1 { color: #28a745; font-size: 24px; }
    p { color: #666; font-size: 16px; margin-bottom: 30px; }
    a { display: inline-block; background-color: #007aff; color: white; padding: 15px 30px; text-decoration: none; border-radius: 8px; font-weight: 500; font-size: 18px; transition: background-color 0.2s; }
    a:hover { background-color: #0056b3; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Configuration Sent!</h1>
    <p>Your device will now attempt to connect to the new Wi-Fi network.</p>
    <a href=")rawliteral" +
         redirectUrl + R"rawliteral(">Return to App</a>
  </div>
</body>
</html>
)rawliteral";
}

String HyperwisorIOT::getErrorHtml(String errorMessage)
{
  String redirectUrl = "hypervisorv4://provisioning?status=error&message=" + errorMessage;
  String decodedMessage = errorMessage;
  decodedMessage.replace("_", " ");

  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Provisioning Error</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f0f2f5; text-align: center; padding: 20px; }
    .card { background: white; padding: 40px; border-radius: 12px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); max-width: 400px; }
    h1 { color: #dc3545; font-size: 24px; }
    p { color: #666; font-size: 16px; margin-bottom: 30px; }
    a { display: inline-block; background-color: #6c757d; color: white; padding: 15px 30px; text-decoration: none; border-radius: 8px; font-weight: 500; font-size: 18px; transition: background-color 0.2s; }
    a:hover { background-color: #5a6268; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Provisioning Failed</h1>
    <p>Error: )rawliteral" +
         decodedMessage + R"rawliteral(</p>
    <a href=")rawliteral" +
         redirectUrl + R"rawliteral(">Return to App</a>
  </div>
</body>
</html>
)rawliteral";
}

void HyperwisorIOT::getcredentials()
{

  delay(1000);
  preferences.begin("wifi-creds", false);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  deviceid = preferences.getString("deviceid", "");
  userid = preferences.getString("userid", "");
  email = preferences.getString("email", "");
  productid = preferences.getString("productid", "");
  versionid = preferences.getString("firmware", "");
  macid = WiFi.macAddress();
  preferences.end();

  Serial.println("Loaded credentials:");
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(deviceid);
}

void HyperwisorIOT::connectToWiFi()
{
  const char *hostname = "NIKOLAINDUSTRY_Device";
  WiFi.setHostname(hostname);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.println("Connecting to WiFi...");
  unsigned long start = millis();
  const unsigned long timeout = 30000;

  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
    realtime.begin(deviceid.c_str());
    setupMessageHandler();
  }
  else
  {
    Serial.println("\nFailed to connect. Switching to AP Mode.");
    startAPMode();
  }
}

void HyperwisorIOT::setupMessageHandler()
{
  realtime.setOnMessageCallback([this](JsonObject &msg)
                                {
                                  if (!msg.containsKey("payload"))
                                    return;

                                  JsonObject payload = msg["payload"];
                                  JsonArray commands = payload["commands"];
                                  this->newtarget = msg["from"] | "";

                                  for (JsonObject commandObj : commands)
                                  {
                                    const char *command = commandObj["command"];

                                    if (strcmp(command, "GPIO_MANAGEMENT") == 0)
                                    {
                                      JsonArray actions = commandObj["actions"];

                                      for (JsonObject actionObj : actions)
                                      {
                                        const char *action = actionObj["action"];
                                        JsonObject params = actionObj["params"];

                                        // Extract parameters
                                        int gpio = atoi(params["gpio"] | "0");
                                        String pinmodeStr = params["pinmode"] | "OUTPUT";
                                        String statusStr = params["status"] | "LOW";

                                        // Resolve pin mode
                                        int mode = OUTPUT;
                                        if (pinmodeStr == "INPUT")
                                          mode = INPUT;
                                        else if (pinmodeStr == "INPUT_PULLUP")
                                          mode = INPUT_PULLUP;

                                        // Resolve output state
                                        int value = LOW;
                                        if (statusStr == "HIGH")
                                          value = HIGH;

                                        // Apply pin configuration
                                        pinMode(gpio, mode);

                                        if (strcmp(action, "ON") == 0 || strcmp(action, "OFF") == 0)
                                        {
                                          digitalWrite(gpio, value);
                                          Serial.printf("Pin %d set to %s with mode %s\n", gpio, statusStr.c_str(), pinmodeStr.c_str());
                                        } // next action inside command
                                      }
                                    }
                                    else if (strcmp(command, "OTA") == 0)
                                    {
                                      JsonArray actions = commandObj["actions"];

                                      for (JsonObject actionObj : actions)
                                      {

                                        const char *action = actionObj["action"];
                                        JsonObject params = actionObj["params"];

                                        if (strcmp(action, "ota_update") == 0)
                                        {

                                          const char *otaUrl = params["url"];
                                          const char *version = params["version"];
                                          this->versionid = String(version);
                                          if (otaUrl != nullptr)
                                          {
                                            performOTA(otaUrl); // Trigger OTA update
                                          }
                                          else
                                          {
                                            Serial.println("Invalid OTA URL received.");
                                          }
                                        }
                                      }
                                    } // next command
                                  }

                                  if (userCommandCallback)
                                  {
                                    userCommandCallback(msg); // Pass entire message to the user's custom logic
                                  } });
}

void HyperwisorIOT::setUserCommandHandler(UserCommandCallback cb)
{
  userCommandCallback = cb;
}

void HyperwisorIOT::sendTo(const String &targetId, std::function<void(JsonObject &)> payloadBuilder)
{
  DynamicJsonDocument doc(512); // Adjust size as needed
  JsonObject root = doc.to<JsonObject>();

  root["targetId"] = targetId;

  JsonObject payload = root.createNestedObject("payload");
  payloadBuilder(payload); // Let user fill the payload

  realtime.sendJson(root); // ✅ Pass JsonObject directly
}

HyperTaskManager &HyperwisorIOT::getTaskManager()
{
  return taskManager;
}

void HyperwisorIOT::saveGPIOState(int pin, int state)
{
  gpioPreferences.begin("gpio-states", false);
  gpioPreferences.putInt(("pin_" + String(pin)).c_str(), state);
  gpioPreferences.end();
}

int HyperwisorIOT::loadGPIOState(int pin)
{
  gpioPreferences.begin("gpio-states", false);
  return gpioPreferences.getInt(("pin_" + String(pin)).c_str(), LOW);
  gpioPreferences.end();
}

void HyperwisorIOT::restoreAllGPIOStates()
{
  gpioPreferences.begin("gpio-states", false);
  for (int pin = 0; pin < 40; pin++)
  {
    String key = "pin_" + String(pin);
    if (gpioPreferences.isKey(key.c_str()))
    {
      int state = gpioPreferences.getInt(key.c_str(), LOW);
      pinMode(pin, OUTPUT);
      digitalWrite(pin, state);
      Serial.printf("Restored pin %d to state %d\n", pin, state);
    }
  }
  gpioPreferences.end();
}

void HyperwisorIOT::performOTA(const char *otaUrl)
{
  String firmwarefeedback;
  StaticJsonDocument<512> firmwarefeedbackDoc; // Increased buffer size

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  Serial.printf("Attempting to download OTA file from %s\n", otaUrl);

  http.begin(client, otaUrl);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK)
  {
    Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    realtime.sendTo(newtarget, [&](JsonObject &payload)
                    {
      payload["status"] = "OTA_Download_Failed";
      payload["value"] = http.errorToString(httpCode).c_str(); });
    http.end();
    return;
  }

  int contentLength = http.getSize();
  Serial.printf("Content length: %d\n", contentLength);

  if (contentLength <= 0)
  {
    Serial.println("No content in the OTA update file.");
    realtime.sendTo(newtarget, [&](JsonObject &payload)
                    {
      payload["status"] = "OTA_Download_Failed";
      payload["value"] = "No content in OTA file."; });
    http.end();
    return;
  }

  Serial.println("Starting OTA update...");

  realtime.sendTo(newtarget, [&](JsonObject &payload)
                  { payload["status"] = "OTA_Update_Started"; });

  if (!Update.begin(contentLength))
  {
    Serial.println("Not enough space for OTA update!");
    realtime.sendTo(newtarget, [&](JsonObject &payload)
                    {
      payload["status"] = "OTA_Download_Failed";
      payload["value"] = "Not enough space!"; });
    http.end();
    return;
  }

  WiFiClient &stream = http.getStream();
  size_t written = Update.writeStream(stream);

  Serial.printf("Written %d bytes\n", written);

  if (written == contentLength && Update.end() && Update.isFinished())
  {
    Serial.println("OTA update successfully completed.");

    preferences.begin("wifi-creds", false);
    preferences.putString("firmware", versionid);
    preferences.end();

    realtime.sendTo(newtarget, [&](JsonObject &payload)
                    {
      payload["status"] = "OTA_Update_Completed";
      payload["value"] = "Rebooting"; });

    delay(2000);
    ESP.restart();
  }
  else
  {
    Serial.println("OTA update failed!");
    realtime.sendTo(newtarget, [&](JsonObject &payload)
                    {
      payload["status"] = "OTA_Update_Failed";
      payload["value"] = Update.errorString(); });
  }

  http.end();
}
