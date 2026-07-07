
[![Platform](https://img.shields.io/badge/hyperwisor%20IoT%20platform-black?style=for-the-badge&logo=nodedotjs)](https://www.hyperwisor.com/)
[![Platform](https://img.shields.io/badge/espressif%20esp32%20patform-gray?style=for-the-badge&logo=espressif)](https://www.espressif.com/en/products/socs/esp32)
# Hyperwisor-IOT Arduino Library
[![License](https://img.shields.io/badge/license-Proprietary-red)](LICENSE)
[![Status](https://img.shields.io/badge/status-In--Development-yellow)]()
[![Arduino](https://img.shields.io/badge/Arduino%20IDE-%3E%3D1.8-blue)](https://www.arduino.cc/en/software)

**Hyperwisor IOT** is a powerful abstraction layer for ESP32-based IoT devices. It handles Wi-Fi provisioning, real-time communication, OTA updates, and GPIO management out of the box. Built on top of the [`nikolaindustry-realtime`](https://github.com/your-org/nikolaindustry-realtime) protocol, it helps developers build smart, connected devices with minimal code.

**Hyperwisor IOT** is a robust abstraction layer for **ESP32-based IoT devices**, enabling:

- Seamless Wi-Fi provisioning
- Real-time communication
- OTA firmware updates
- GPIO & task management
- Persistent configuration
- Fully structured JSON command execution

Built on the powerful [`nikolaindustry-realtime`](https://github.com/your-org/nikolaindustry-realtime) protocol.

## 📦 Features

* 🚀 Automatic Wi-Fi connection using stored credentials
* 📶 AP-mode fallback + web-based provisioning page
* 🌐 Real-time communication using `nikolaindustry-realtime`
* 📩 Built-in JSON command parser with custom extensibility
* 🛠️ GPIO control via commands (`pinMode`, `digitalWrite`)
* 🔁 Continuous background loop with nikolaindustry-realtime + HTTP handling
* 🔧 User command handler support via lambda functions
* 🌍 Built-in DNS redirection when in AP mode
* 🔄 OTA firmware update with version tracking
* 🧠 Smart command routing via `from` → `sendTo()` pairing
* ✅ Command response feedback through real-time socket
* 🔐 Preferences-based persistent storage
* 🛡️ **Secure by default** — unique on-chip P-256 device identity (Hyperwisor Secure Channel)

---

## 🔧 Installation

1. Clone or download this repository.
2. Place the folder into your Arduino `libraries/` directory.
3. Include the library in your sketch:

   ```cpp
   #include <hyperwisor-iot.h>
   ```



## 🌐 Command Structure

Supports rich JSON messages from the server:

```json
{
  "from": "controller-xyz",
  "payload": {
    "commands": [
      {
        "command": "GPIO_MANAGEMENT",
        "actions": [
          {
            "action": "ON",
            "params": {
              "gpio": 12,
              "pinmode": "OUTPUT",
              "status": "HIGH"
            }
          }
        ]
      }
    ]
  }
}
```

## 🧠 Internals

On `hyper.begin()`:

1. Tries to connect to saved Wi-Fi
2. Starts AP-mode if not configured
3. Sets up DNS and HTTP provisioning
4. Initializes nikolaindustry-realtime connection
5. Loads `deviceid`, `ssid`, `email`, `productid`, and `versionid` from `Preferences`
6. Registers default and user-provided command handlers

---

## 🔐 Security (Hyperwisor Secure Channel)

**As of v2.0.0, security is ON by default.** Each device generates a unique
**ECDSA P‑256 keypair on‑chip** (stored in NVS). The **private key never leaves the
device** — only the public key is ever shared. On connect, the device proves its
identity to the Hyperwisor secure relay with a challenge–response handshake, so
nobody can impersonate your hardware.

### What you get for free
- `hyper.begin()` **automatically enables** the secure channel — no extra code.
- The device connects to the secure relay and **authenticates before any data flows**.
- The keypair is generated once and persisted in NVS; regular reboots keep the same identity.

### Onboarding registers the public key
During Wi‑Fi provisioning, the device hands its **public key** back to the Hyperwisor
app (in the provisioning acknowledgment). The app registers it to your account so the
relay knows the device is really yours.

> **Important:** a device must be **onboarded through the Hyperwisor app** for its key
> to be registered. A freshly flashed device that connects straight to saved Wi‑Fi
> (skipping provisioning) won't have its key registered yet and will be rejected as
> `unknown device` until you (re‑)provision it.

### API
| Method | Purpose |
|--------|---------|
| `hyper.begin()` | Auto‑enables security (default). |
| `hyper.enableSecurity("host", port)` | Override the relay host/port (advanced). |
| `hyper.disableSecurity()` | **Opt out** — call **before** `begin()` to use the legacy unauthenticated relay. |
| `hyper.getPublicKeyBase64()` | The device's public key (base64 raw P‑256). |
| `hyper.getDeviceId()` | The device's ID. |

### Opting out (legacy mode)
```cpp
void setup() {
  hyper.disableSecurity();   // MUST be called before begin()
  hyper.begin();             // now uses the legacy unauthenticated relay
}
```

### Upgrading from v1.x (breaking change)
v2.0.0 is secure‑by‑default. Devices already in the field must **register their key**
(re‑onboard through the app) to authenticate on the secure relay. Devices you can't
re‑onboard yet can call `hyper.disableSecurity()` to stay on the legacy relay until
migrated. OTA updates continue to work in both modes.

---

## 📚 Dependencies

### Required: Install via Arduino Library Manager
These libraries must be installed before using Hyperwisor-IOT:

| Library | Author | Install via |
|---------|--------|-------------|
| [ArduinoJson](https://arduinojson.org/) | Benoit Blanchon | Library Manager |
| [WebSockets](https://github.com/Links2004/arduinoWebSockets) | Markus Sattler | Library Manager |

**How to install:**
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries...**
3. Search for `ArduinoJson` and click **Install**
4. Search for `WebSockets` and click **Install**

### Required: ESP32 Board Package
This library only works with ESP32. Install the ESP32 board package:
1. Go to **File → Preferences**
2. Add this URL to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools → Board → Boards Manager...**
4. Search for `esp32` and install **esp32 by Espressif Systems**

### Built-in (No installation needed)
These libraries come with the ESP32 board package:
- WiFi, WebServer, HTTPClient, WiFiClientSecure
- Preferences, Update, DNSServer, Wire



## 🧾 License

# Hyperwisor IoT Arduino Library License

Copyright (c) 2025 NIKOLAINDUSTRY

## License Type: Proprietary - All Rights Reserved

This software and associated files (the "Library") are the exclusive property of **NIKOLAINDUSTRY**. You are **NOT** permitted to copy, modify, distribute, sublicense, or reverse engineer any part of this code without express written permission from NIKOLAINDUSTRY.

### You MAY:
- Use this Library **exclusively** with NIKOLAINDUSTRY hardware or software platforms.
- Integrate the Library into closed-source commercial or industrial applications developed **in partnership with** NIKOLAINDUSTRY.
- Contact NIKOLAINDUSTRY for licensing terms to enable distribution or broader use.

### You MAY NOT:
- Redistribute, sell, sublicense, or disclose this Library or any derivative works.
- Modify or decompile the source code for any purpose other than evaluation or integration with authorized systems.
- Use this Library in any open-source project, public-facing repository, or third-party product without prior written approval.

## Disclaimers

- **NO WARRANTY**: This software is provided "as is" without any warranties of any kind, whether express or implied, including but not limited to warranties of merchantability or fitness for a particular purpose.

- **NO LIABILITY**: In no event shall NIKOLAINDUSTRY, its employees, or its affiliates be liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including hardware damage, data loss, or system failure) arising in any way from the use or inability to use this Library.

- **NOT FOR CRITICAL SYSTEMS**: This Library is **not certified** for use in life-support, nuclear, military, or other safety-critical systems. Use in such environments is **strictly prohibited**.

## Enforcement

Unauthorized use, duplication, or distribution of this Library may result in civil and criminal penalties and will be prosecuted to the maximum extent possible under law.

---

For commercial licensing, OEM partnerships, or distribution rights, contact:

**NIKOLAINDUSTRY**  
Email: support@nikolaindustry.com  
Website: [https://nikolaindustry.com](https://nikolaindustry.com)
---

## 🤝 Contribute

Suggestions, bug fixes, and pull requests are welcome. Help evolve the library to support more protocols and automation use-cases!

---


