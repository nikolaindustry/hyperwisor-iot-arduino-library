# Hyperwisor IoT — Arduino Library

[![Arduino Library Manager](https://img.shields.io/badge/Arduino%20Library%20Manager-3.3.0-00979D?style=flat-square&logo=arduino&logoColor=white)](https://www.arduino.cc/reference/en/libraries/)
[![Platform](https://img.shields.io/badge/platform-ESP32-E7352C?style=flat-square&logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32)
[![Secure channel](https://img.shields.io/badge/identity-ECDSA%20P--256-2F6F4E?style=flat-square)](#security)
[![License](https://img.shields.io/badge/license-proprietary-6B7280?style=flat-square)](License.md)

Connect an ESP32 to the [Hyperwisor](https://www.hyperwisor.com/) platform in a
few lines. The library handles Wi-Fi provisioning, an authenticated real-time
channel, OTA updates, GPIO control and JSON command execution, so a sketch can
be about the product rather than the plumbing.

Every device holds a **unique ECDSA P-256 keypair generated on-chip**. The
private key never leaves the ESP32, and the device proves its identity to the
Hyperwisor Secure Channel before any data flows.

---

## Install

**Arduino IDE** — Tools → Manage Libraries, search `Hyperwisor-IOT`, install.

**arduino-cli**

```bash
arduino-cli lib install Hyperwisor-IOT
```

You also need the ESP32 board package (`esp32` by Espressif Systems) and two
libraries, both from Library Manager:

| Library | Author |
|---|---|
| [ArduinoJson](https://arduinojson.org/) | Benoit Blanchon |
| [WebSockets](https://github.com/Links2004/arduinoWebSockets) | Markus Sattler |

Everything else — `WiFi`, `WebServer`, `HTTPClient`, `WiFiClientSecure`,
`Preferences`, `Update`, `DNSServer`, `Wire` — ships with the ESP32 board
package.

---

## Quick start

```cpp
#include <hyperwisor-iot.h>

HyperwisorIOT device;

void setup() {
  Serial.begin(115200);
  device.begin();                       // Wi-Fi, identity, secure channel
  Serial.println("Device ID: " + device.getDeviceId());
}

void loop() {
  device.loop();                        // keeps the connection alive
}
```

That is a complete, connectable device. `begin()` restores saved credentials,
brings up the on-chip key and authenticates; `loop()` pumps messages and drives
reconnection.

---

## Getting a device onto your account

A device cannot simply connect. The relay only accepts devices whose public key
is registered to an account, so the first boot goes through provisioning:

1. **First boot** — no stored credentials, so the device raises its own access
   point (`NIKOLAINDUSTRY_Setup…`, password `0123456789`) and serves a setup
   page at `192.168.4.1`.
2. **Hand over Wi-Fi** — the Hyperwisor mobile app sends your network details to
   that page. The device replies with its **public key**, and the app registers
   it against your account.
3. **Restart** — the device joins your network and authenticates on the secure
   channel.

> A device flashed with Wi-Fi credentials already in NVS skips provisioning,
> never registers its key, and is refused as `unknown device`. Re-provision it
> through the app to fix this. There is no insecure fallback to wait for.

Change the access point with `setProvisioningAP()` before `begin()` if the
defaults do not suit your product.

---

## Receiving commands

The platform sends structured JSON. One message may carry several commands, each
with several actions:

```json
{
  "from": "controller-xyz",
  "payload": {
    "commands": [
      {
        "command": "GPIO_MANAGEMENT",
        "actions": [
          { "action": "ON", "params": { "gpio": 12, "pinmode": "OUTPUT", "status": "HIGH" } }
        ]
      }
    ]
  }
}
```

`GPIO_MANAGEMENT` and OTA are handled for you. Anything else reaches your
handler:

```cpp
device.setUserCommandHandler([](const String &from, JsonObject &cmd) {
  String command = cmd["command"].as<String>();
  if (command == "relay_control") {
    // act on cmd["actions"], then reply to `from`
  }
});
```

Replies and telemetry go back the same way:

```cpp
device.sendTo(from, [&](JsonObject &p) {
  p["status"] = "ok";
  p["relay"]  = 3;
});
```

---

## Connection behaviour

Since **3.3.0** reconnection backs off and spreads, rather than retrying at a
fixed interval.

| Situation | What happens |
|---|---|
| Ordinary drop | Retry after 1–3 s, then 2 s, 4 s, 8 s … capped at 60 s |
| Jitter | Each retry lands at a random point in a window as wide as the wait |
| Identity rejected | Flat 5 s retry, no escalation |
| Handshake completes | The backoff resets — only `auth_ok` counts as success |

The jitter matters once more than one device exists. A fixed interval means
every device reconnects in lockstep after a server restart, and the resulting
burst keeps the server down. A window that widens with the delay puts the
largest spread exactly where the crowd is biggest.

On a single device this is a trade: after a long outage, recovery can take up to
about 90 seconds instead of retrying every 2. An ordinary blip is unaffected.

Watch it on the serial monitor:

```
🔴 relay disconnected
⏳ next reconnect in 3184ms (closed, attempt 1)
🔐 HSC: signed challenge → sent auth
🔐 HSC: authenticated
```

---

## Security

Security is not a setting. `begin()` generates (or loads) the device's P-256
keypair in NVS, connects to the Hyperwisor Secure Channel and completes a
challenge–response handshake before the sketch can send anything.

- The **private key never leaves the chip**. Only the public key is ever shared.
- The keypair survives reboots and OTA updates, so the identity is stable.
- Frames written before the handshake finishes are held rather than sent, so a
  publishing loop that starts early cannot break the connection.

```cpp
device.getPublicKeyBase64();   // base64 raw P-256, safe to print or copy
device.getDeviceId();
```

---

## API

| Method | Purpose |
|---|---|
| `begin()` | Credentials, identity, secure channel, command handlers |
| `loop()` | Pump messages and drive reconnection. Call every iteration |
| `setUserCommandHandler(cb)` | Handle commands the library does not |
| `setConnectionStatusHandler(cb)` | `true` on `auth_ok`, `false` on disconnect |
| `sendTo(targetId, builder)` | Send a JSON payload to a peer |
| `send_Sensor_Data_logger(targetId, configId, {…})` | Push readings to a data logger |
| `setFirmwareVersion(v)` | Report a version the platform can compare for OTA |
| `setProvisioningAP(ssid, password)` | Name the setup access point |
| `setCredentials(ssid, pass, deviceId, userId)` | Set credentials in code |
| `hasCredentials()` / `clearCredentials()` | Inspect or wipe stored credentials |
| `saveGPIOState(pin, state)` / `restoreAllGPIOStates()` | Persist outputs across reboots |
| `getDeviceId()` / `getUserId()` / `getPublicKeyBase64()` | Identity |
| `setRelayHost(host, port)` | Point at a different relay. Call before `begin()` |

---

## Upgrading from 2.x

`enableSecurity()` and `disableSecurity()` were removed in 3.0.0. They existed
when unauthenticated connections were still possible; they are not, so an API
offering the choice was misleading.

- `disableSecurity()` → delete the line.
- `enableSecurity("host", port)` → `setRelayHost("host", port)`.
- `enableSecurity()` → delete the line; `begin()` does it.

OTA is unaffected. See [CHANGELOG.md](CHANGELOG.md) — releases 3.0.1 through
3.2.0 were never published to Library Manager, so an update from 3.0.0 brings
four versions at once, including a provisioning security fix.

---

## Licence

Proprietary — © NIKOLAINDUSTRY. The library is licensed for use **with the
Hyperwisor platform and NIKOLAINDUSTRY hardware**; redistribution,
sublicensing and derivative works are not permitted without written
permission. Provided without warranty, and **not certified for life-support,
nuclear, military or other safety-critical systems**.

Full terms: [License.md](License.md). For commercial licensing, OEM
partnerships or distribution rights: **support@nikolaindustry.com**.

## Issues

Bug reports and reproducible test cases are welcome through GitHub issues.
Because the licence does not permit derivative works, please open an issue
rather than a pull request, and we will take it from there.
