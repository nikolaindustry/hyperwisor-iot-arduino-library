// HSC v1 — Hyperwisor Secure Channel (device side).
//
// On-chip P-256 identity for the ESP32. The private key is generated ON the chip
// at first boot and stored in NVS; it never leaves the device. Only the public
// key is ever shared (registered with the platform during onboarding).
//
// This module answers the relay's connection challenge by signing it, proving the
// device is genuine — without ever transmitting a secret. Built on the ESP32's
// built-in mbedTLS (ECDSA P-256 + SHA-256). The wire formats match the relay's
// utils/hsc.js byte-for-byte:
//   - public key : raw uncompressed point 0x04||X||Y (65 bytes), base64
//   - signature  : raw r||s (IEEE-P1363, 64 bytes), base64
//   - message    : "HYPERWISOR-HSC-v1" \x1f role \x1f deviceId \x1f channelId
//                  \x1f nonce \x1f ts   (UTF-8, then SHA-256, then ECDSA)
//
// See ../../Hyperwisor-v4/security/hsc-v1-spec.md.

#ifndef HYPERWISOR_HSC_H
#define HYPERWISOR_HSC_H

#include <Arduino.h>

class HyperwisorHSC
{
public:
  HyperwisorHSC();

  // Load the device keypair from NVS, or generate + persist one on first boot.
  // Safe to call every boot. Returns true if a usable keypair is available.
  bool begin();

  // Base64 of the raw 65-byte uncompressed public key — what you register with
  // the platform (relay-register-device) during onboarding.
  String getPublicKeyBase64();

  // Sign a relay challenge. Returns base64 of the raw 64-byte signature, or ""
  // on failure. role is "device"; channelId equals deviceId for the device leg.
  String signChallenge(const String &deviceId, const String &nonce, const String &ts);

  // Erase the stored keypair (factory reset). The next begin() regenerates one —
  // note this creates a NEW identity that must be re-registered.
  void erase();

  bool hasKeypair() const { return _ready; }

private:
  bool _ready = false;
  uint8_t _priv[32];     // private scalar d
  uint8_t _pub[65];      // 0x04 || X || Y

  bool loadFromNVS();
  bool generateAndStore();
  // Build the canonical HSC message into `out` (caller-sized buffer via String).
  static String buildMessage(const String &role, const String &deviceId,
                             const String &channelId, const String &nonce,
                             const String &ts);
};

#endif // HYPERWISOR_HSC_H
