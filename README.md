
# hyperwisor-iot-arduino-library

**HyperwisorIOT** is a powerful abstraction layer for ESP32-based IoT devices. It handles Wi-Fi provisioning, real-time communication, OTA updates, and GPIO management out of the box. Built on top of the [`nikolaindustry-realtime`](https://github.com/your-org/nikolaindustry-realtime) protocol, it helps developers build smart, connected devices with minimal code.


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

---

## 🔧 Installation

1. Clone or download this repository.
2. Place the folder into your Arduino `libraries/` directory.
3. Include the library in your sketch:

   ```cpp
   #include <hyperwisor-iot.h>
   ```

---

## 🧪 Example Usage

```cpp
#include <hyperwisor-iot.h>

String from = "";

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();


  hyper.setUserCommandHandler([](JsonObject& msg) {
    if (msg.containsKey("from")) {
      from = msg["from"].as<String>();
      Serial.println("Message from: " + from);
    }


    if (!msg.containsKey("payload")) return;
    JsonObject payload = msg["payload"];
    JsonArray commands = payload["commands"];

    for (JsonObject commandObj : commands) {
      const char* command = commandObj["command"];

      if (strcmp(command, "CUSTOM_COMMAND") == 0) {
        Serial.println("Handling CUSTOM_COMMAND in .ino");

        JsonArray actions = commandObj["actions"];
        for (JsonObject actionObj : actions) {
          const char* action = actionObj["action"];
          JsonObject params = actionObj["params"];

          // 👉 Do custom logic here
          Serial.printf("Custom action: %s\n", action);
          if (params.containsKey("value")) {
            Serial.printf("Value: %s\n", params["value"].as<const char*>());
          }
        }
      }
    }
  });


  hyper.sendTo(from, [](JsonObject& payload) {
    JsonArray commands = payload.createNestedArray("commands");

    JsonObject command = commands.createNestedObject();
    command["command"] = "device";

    JsonArray actions = command.createNestedArray("actions");
    JsonObject action = actions.createNestedObject();
    action["action"] = "ON";

    JsonObject params = action.createNestedObject("params");
    params["gpio"] = 5;
    params["pinmode"] = "OUTPUT";
    params["status"] = "HIGH";
  });

  // Setup a GPIO (pin 2) and save its state
 // pinMode(2, OUTPUT);
  //digitalWrite(2, HIGH);

  // hyper.saveGPIOState(2, HIGH);

 // Optional: Restore all previously saved GPIO states on startup
  hyper.restoreAllGPIOStates();

  // Add a blink task on pin 5 (blink 5 times with 300ms on/off)
  hyper.getTaskManager().addBlink(5, 300, 300, 5);

  // Add a fade task on pin 18 from 0 to 255 brightness over 3 seconds
  hyper.getTaskManager().addFade(18, 0, 255, 3000);

  // Add a pulse task on pin 19 to go HIGH for 2 seconds then revert
  hyper.getTaskManager().addPulse(19, 2000, HIGH);

  Serial.println("All tasks started.");
}

void loop() {
  hyper.loop();
}

```

---

## ⚙️ Wi-Fi Provisioning

If the device can't connect to Wi-Fi, it starts in **AP Mode**, hosting an HTTP server and DNS server for provisioning. Access `192.168.4.1` and submit the form to set:

* `ssid`
* `password`
* `target_id`

Upon success, the ESP32 saves credentials in NVS and restarts.

---

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
      },
      {
        "command": "OTA",
        "actions": [
          {
            "action": "ota_update",
            "params": {
              "url": "https://firmware-server/update.bin",
              "version": "v2.1.0"
            }
          }
        ]
      }
    ]
  }
}
```

---

## 🔄 OTA Firmware Update

Send an `OTA` command with `ota_update` action and a valid `url`. The library:

* Downloads the binary via HTTPS
* Validates and applies update
* Sends status messages back to the `from` target
* Restarts the device after update

---

## 🧠 Internals

On `hyper.begin()`:

1. Tries to connect to saved Wi-Fi
2. Starts AP-mode if not configured
3. Sets up DNS and HTTP provisioning
4. Initializes nikolaindustry-realtime connection
5. Loads `deviceid`, `ssid`, `email`, `productid`, and `versionid` from `Preferences`
6. Registers default and user-provided command handlers

---

## 📤 Sending Data

Use the `sendTo()` method to respond or send commands:

```cpp
hyper.sendTo(targetId, [](JsonObject& payload) {
  JsonArray commands = payload.createNestedArray("commands");
  // build your message
});
```

---

## 📚 Dependencies

* [`WiFi.h`](https://www.arduino.cc/en/Reference/WiFi)
* [`ArduinoJson`](https://arduinojson.org/)
* [`nikolaindustry-realtime`](https://github.com/your-org/nikolaindustry-realtime)
* [`Preferences`](https://www.arduino.cc/en/Reference/Preferences)
* [`Update`](https://www.arduino.cc/en/Reference/Update)
* [`DNSServer`](https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/src/DNSServer.h)

Install all via Arduino Library Manager.

---

## ✍️ Extend with Custom Logic

Implement in `setUserCommandHandler()` or customize inside the library to add:

* I2C or RS485 actions
* Relay and GPIO sequencing
* Device diagnostics and self-tests
* Integration with sensors (DHT, BMP, etc.)

---

## 🔐 Persistent Storage Keys

| Key        | Purpose                 |
| ---------- | ----------------------- |
| `ssid`     | Wi-Fi SSID              |
| `password` | Wi-Fi Password          |
| `deviceid` | Unique ID for backend   |
| `userid`   | Optional user field     |
| `email`    | Optional user email     |
| `firmware` | Last known firmware ver |

---

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


