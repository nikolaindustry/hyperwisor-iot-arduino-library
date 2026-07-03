#ifndef NIKOLAINDUSTRY_REALTIME_H
#define NIKOLAINDUSTRY_REALTIME_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <functional>

class nikolaindustryrealtime
{
public:
  nikolaindustryrealtime();
  void begin(const char *deviceId);
  void loop();
  void sendJson(const JsonObject &json);
  void sendTo(const String &targetId, std::function<void(JsonObject &)> payloadBuilder);

  void setOnMessageCallback(std::function<void(JsonObject &)> callback);
  void setOnConnectionStatusChange(std::function<void(bool)> callback);
  bool isNikolaindustryRealtimeConnected();

  // --- HSC v1 (Hyperwisor Secure Channel) ---
  // Point the transport at a different relay host (e.g. the secured relay).
  // Defaults to the current production host if never called.
  void setHost(const String &host, uint16_t port = 443);

  // Enable the HSC handshake. `signer(nonce, ts)` must return the base64 raw
  // signature over the challenge (wired to HyperwisorHSC::signChallenge). When
  // enabled, the transport is not considered "connected" (no status callback,
  // no message delivery) until the relay returns auth_ok.
  void enableHSC(std::function<String(const String &nonce, const String &ts)> signer);

private:
  WebSocketsClient webSocket;
  String deviceId;
  bool _isConnected = false;

  // HSC state
  String _host = "nikolaindustry-realtime.onrender.com";
  uint16_t _port = 443;
  bool _hscEnabled = false;
  bool _authenticated = false;
  std::function<String(const String &, const String &)> _authSigner;
  void handleChallenge(JsonObject &obj); // sign + reply
  bool handleHscControl(JsonObject &obj); // returns true if it was an HSC frame

  std::function<void(JsonObject &)> onMessageCallback;
  std::function<void(bool)> onConnectionStatusChange;

  void connect();
};

#endif
