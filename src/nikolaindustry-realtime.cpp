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
        _warnedPreAuthSend = false;
        if (_hscEnabled) {
          // Do NOT report connected yet — wait for the HSC challenge → auth_ok.
          Serial.println("🔐 HSC: awaiting challenge...");
        } else {
          _isConnected = true;
          if (onConnectionStatusChange) onConnectionStatusChange(true);
        }
        break;
      case WStype_DISCONNECTED:
        Serial.println("🔴 relay disconnected");
        _isConnected = false;
        _authenticated = false;
        _warnedPreAuthSend = false;
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

  // Recovery time after a dropped connection.
  //
  // At 5000 a single missed attempt cost eleven seconds of downtime: the first
  // retry at +5s failed, the second at +10s opened, then SSL and the handshake.
  // For a device driving a pump or a light, ten seconds of "offline" is the
  // difference between a glitch and a complaint.
  //
  // TRADE-OFF, know it before raising the fleet size: WebSocketsClient has no
  // exponential backoff, so this interval is fixed. If the relay goes down,
  // every device retries every two seconds for as long as the outage lasts.
  // That is fine at today's scale and becomes a thundering herd at thousands of
  // devices — at which point this needs real backoff (2s, 4s, 8s, capped),
  // which has to be built rather than configured.
  webSocket.setReconnectInterval(2000);
  
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

void nikolaindustryrealtime::sendJsonRaw(const JsonObject &json)
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

void nikolaindustryrealtime::sendJson(const JsonObject &json)
{
  // Nothing may go out before the relay has said auth_ok.
  //
  // A sketch publishes on its own timer and knows nothing about the handshake,
  // so a sensor reading regularly landed on the relay in the window between
  // "sent auth" and "authenticated". The relay treated that stray frame as a
  // failed handshake and closed the connection, so the device reconnected into
  // the same race — a loop that looked like the relay rejecting a valid device
  // (observed 2026-08-20).
  //
  // The relay now drops such frames instead of closing, but the device should
  // not be sending them in the first place: pre-auth the socket is open and
  // unauthenticated, and anything written to it is guaranteed to be discarded.
  if (_hscEnabled && !_authenticated)
  {
    if (!_warnedPreAuthSend)
    {
      _warnedPreAuthSend = true;   // once per connection; a publishing loop must not flood serial
      Serial.println("⏳ send held: not authenticated yet (frame dropped)");
    }
    return;
  }
  sendJsonRaw(json);
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
  sendJsonRaw(out.as<JsonObject>());   // the auth frame itself — must bypass the pre-auth guard
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
