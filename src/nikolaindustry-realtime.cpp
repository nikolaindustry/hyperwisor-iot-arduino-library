#include "nikolaindustry-realtime.h"

nikolaindustryrealtime::nikolaindustryrealtime() {}

void nikolaindustryrealtime::begin(const char *_deviceId)
{
  deviceId = _deviceId;

  if (WiFi.status() == WL_CONNECTED)
  {
    connect();
  }
  else
  {
    Serial.println("❌ WiFi not connected. WebSocket not started.");
  }
}

void nikolaindustryrealtime::connect()
{
  Serial.printf("🔄 Connecting to %s ...\n", _host.c_str());

  webSocket.beginSSL(_host.c_str(), _port, ("/?id=" + deviceId).c_str());

  webSocket.onEvent([this](WStype_t type, uint8_t *payload, size_t length)
                    {
    DynamicJsonDocument doc(2048);
    switch (type) {
      case WStype_CONNECTED:
        Serial.println("🟢 transport socket open");
        _authenticated = false;
        if (_hscEnabled) {
          // Do NOT report connected yet — wait for the HSC challenge → auth_ok.
          Serial.println("🔐 HSC: awaiting challenge...");
        } else {
          _isConnected = true;
          if (onConnectionStatusChange) onConnectionStatusChange(true);
        }
        break;
      case WStype_DISCONNECTED:
        Serial.println("🔴 nikolaindustry-realtime disconnected");
        _isConnected = false;
        _authenticated = false;
        if (onConnectionStatusChange) onConnectionStatusChange(false);
        break;
      case WStype_TEXT:
        if (!deserializeJson(doc, payload, length)) {
          JsonObject obj = doc.as<JsonObject>();
          // Intercept HSC handshake frames (challenge / auth_ok / auth_fail).
          if (_hscEnabled && handleHscControl(obj)) break;
          // Before auth, drop everything else.
          if (_hscEnabled && !_authenticated) break;
          if (onMessageCallback) onMessageCallback(obj);
        }
        break;
      case WStype_PING:
        // Ping received from server, pong will be sent automatically
        Serial.printf("📡 [%lu] PING received from server\n", millis());
        break;
      case WStype_PONG:
        // Pong received - connection is verified alive
        Serial.printf("📡 [%lu] PONG received - connection alive ✓\n", millis());
        break;
      case WStype_ERROR:
        Serial.println("❌ WebSocket error occurred");
        _isConnected = false;
        break;
      default:
        break;
    } });

  // Set reconnect interval for automatic reconnection attempts
  webSocket.setReconnectInterval(5000);
  
  // CRITICAL: Enable heartbeat to detect zombie connections
  // Ping every 15 seconds, timeout after 3 seconds, disconnect after 2 failed pongs
  webSocket.enableHeartbeat(15000, 3000, 2);
}

void nikolaindustryrealtime::loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    webSocket.loop();
  }
}

void nikolaindustryrealtime::sendJson(const JsonObject &json)
{
  String output;
  if (serializeJson(json, output))
  {
    webSocket.sendTXT(output);
  }
  else
  {
    Serial.println("❌ Failed to serialize JSON!");
  }
}

void nikolaindustryrealtime::sendTo(const String &targetId, std::function<void(JsonObject &)> payloadBuilder)
{
  DynamicJsonDocument doc(512);
  doc["targetId"] = targetId;
  JsonObject payload = doc.createNestedObject("payload");
  payloadBuilder(payload);
  sendJson(doc.as<JsonObject>());
}

void nikolaindustryrealtime::setOnMessageCallback(std::function<void(JsonObject &)> callback)
{
  onMessageCallback = callback;
}

void nikolaindustryrealtime::setOnConnectionStatusChange(std::function<void(bool)> callback)
{
  onConnectionStatusChange = callback;
}

bool nikolaindustryrealtime::isNikolaindustryRealtimeConnected()
{
  // When HSC is on, "connected" means authenticated.
  if (_hscEnabled) return webSocket.isConnected() && _authenticated;
  return webSocket.isConnected();
}

void nikolaindustryrealtime::setHost(const String &host, uint16_t port)
{
  _host = host;
  _port = port;
}

void nikolaindustryrealtime::enableHSC(std::function<String(const String &, const String &)> signer)
{
  _authSigner = signer;
  _hscEnabled = true;
  Serial.println("🔐 HSC: handshake enabled on transport");
}

// Handle a challenge: sign it and reply with the auth frame.
void nikolaindustryrealtime::handleChallenge(JsonObject &obj)
{
  String nonce = obj["nonce"].as<String>();
  String ts = obj["ts"].as<String>(); // keep the exact decimal form the relay signed
  if (!_authSigner) { Serial.println("❌ HSC: no signer set"); return; }

  String sig = _authSigner(nonce, ts);
  if (sig.length() == 0) { Serial.println("❌ HSC: signing failed"); return; }

  DynamicJsonDocument out(1024);
  out["hsc"] = "1";
  out["type"] = "auth";
  out["deviceId"] = deviceId;
  out["sig"] = sig;
  sendJson(out.as<JsonObject>());
  Serial.println("🔐 HSC: signed challenge → sent auth");
}

// Returns true if the frame was an HSC control frame (and thus consumed).
bool nikolaindustryrealtime::handleHscControl(JsonObject &obj)
{
  const char *t = obj["type"] | "";
  if (strcmp(t, "challenge") == 0) {
    handleChallenge(obj);
    return true;
  }
  if (strcmp(t, "auth_ok") == 0) {
    _authenticated = true;
    _isConnected = true;
    Serial.println("✅ HSC: authenticated");
    if (onConnectionStatusChange) onConnectionStatusChange(true);
    return true;
  }
  if (strcmp(t, "auth_fail") == 0) {
    Serial.printf("❌ HSC: auth rejected: %s\n", obj["reason"] | "unknown");
    _authenticated = false;
    return true;
  }
  return false;
}
