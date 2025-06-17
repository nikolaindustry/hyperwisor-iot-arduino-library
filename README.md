# Hyperwisor-IOT Arduino Library
[![Platform](https://img.shields.io/badge/hyperwisor%20IoT%20patform-white?style=for-the-badge&logo=nodedotjs)](https://www.hyperwisor.com/)
[![Platform](https://img.shields.io/badge/platform-ESP32-blue)](https://www.espressif.com/en/products/socs/esp32)
[![License](https://img.shields.io/badge/license-Proprietary-red)](LICENSE)
[![Status](https://img.shields.io/badge/status-In--Development-yellow)]()
[![Arduino](https://img.shields.io/badge/Arduino%20IDE-%3E%3D1.8-blue)](https://www.arduino.cc/en/software)

**HyperwisorIOT** is a powerful abstraction layer for ESP32-based IoT devices. It handles Wi-Fi provisioning, real-time communication, OTA updates, and GPIO management out of the box. Built on top of the [`nikolaindustry-realtime`](https://github.com/your-org/nikolaindustry-realtime) protocol, it helps developers build smart, connected devices with minimal code.

**HyperwisorIOT** is a robust abstraction layer for **ESP32-based IoT devices**, enabling:

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

  hyper.restoreAllGPIOStates();
  hyper.getTaskManager().restoreAllTasks();

  hyper.setUserCommandHandler([](JsonObject& msg) {
    if (msg.containsKey("from")) {
      from = msg["from"].as<String>();
      Serial.println("Message from: " + from);
    }

    if (!msg.containsKey("payload")) return;
    JsonObject payload = msg["payload"];
    JsonArray commands = payload["commands"];

    for (JsonObject commandObj : commands) {
      if (strcmp(commandObj["command"], "TASK") != 0) continue;

      JsonArray actions = commandObj["actions"];
      for (JsonObject actionObj : actions) {
        const char* action = actionObj["action"];
        JsonObject params = actionObj["params"];
        int pin = params["pin"] | -1;
        String taskId = actionObj["id"] | "";
        bool immunity = params["immunity"] | false;

        if (strcmp(action, "blink") == 0) {
          hyper.getTaskManager().addBlink(pin, params["on"], params["off"], params["repeat"], taskId, immunity);

        } else if (strcmp(action, "fade") == 0) {
          hyper.getTaskManager().addFade(pin, params["start"], params["end"], params["duration"], taskId, immunity);

        } else if (strcmp(action, "pulse") == 0) {
          hyper.getTaskManager().addPulse(pin, params["duration"], params["state"], taskId, immunity);

        } else if (strcmp(action, "toggle") == 0) {
          hyper.getTaskManager().addToggle(pin, params["interval"], taskId, immunity);

        } else if (strcmp(action, "delay") == 0) {
          hyper.getTaskManager().addDelay(pin, params["delay"], params["state"], taskId, immunity);

        } else if (strcmp(action, "interval") == 0) {
          hyper.getTaskManager().addInterval(pin, params["interval"], [](int pin) {
            Serial.printf("Interval callback on pin %d\n", pin);
          }, taskId, immunity);

        } else if (strcmp(action, "ramp") == 0) {
          hyper.getTaskManager().addRamp(pin, params["start"], params["end"], params["duration"], taskId, immunity);

        } else if (strcmp(action, "pwmsweep") == 0) {
          hyper.getTaskManager().addPWMSweep(pin, params["start"], params["end"], params["step"], params["delay"], taskId, immunity);

        } else if (strcmp(action, "debounce") == 0) {
          hyper.getTaskManager().addDebounce(pin, params["debounce"], [](int pin, int state) {
            Serial.printf("Debounced pin %d state %d\n", pin, state);
          }, taskId, immunity);

        } else if (strcmp(action, "sequence") == 0) {
          JsonArray seq = params["sequence"];
          JsonArray times = params["timings"];
          int len = seq.size();
          int* sequenceArray = new int[len];
          int* timingArray = new int[len];
          for (int i = 0; i < len; i++) {
            sequenceArray[i] = seq[i].as<int>();
            timingArray[i] = times[i].as<int>();
          }
          hyper.getTaskManager().addSequence(pin, sequenceArray, timingArray, len, taskId, immunity);
          delete[] sequenceArray;
          delete[] timingArray;

        } else if (strcmp(action, "timeout_restore") == 0) {
          hyper.getTaskManager().addTimeoutRestore(pin, params["state"], params["timeout"], taskId, immunity);

        } else if (strcmp(action, "cancel") == 0) {
          bool result = hyper.getTaskManager().cancelTaskById(taskId);
          Serial.printf("Cancel task %s: %s\n", taskId.c_str(), result ? "Success" : "Failed");

          hyper.sendTo(from, [taskId, result](JsonObject& payload) {
            JsonObject action = payload.createNestedArray("commands").createNestedObject();
            action["command"] = "task";
            JsonObject cancel = action.createNestedArray("actions").createNestedObject();
            cancel["action"] = "cancel_response";
            JsonObject p = cancel.createNestedObject("params");
            p["id"] = taskId;
            p["success"] = result;
          });

        } else if (strcmp(action, "status") == 0) {
          String status = hyper.getTaskManager().getTaskStatusById(taskId);
          Serial.printf("Status of task %s: %s\n", taskId.c_str(), status.c_str());

          hyper.sendTo(from, [taskId, status](JsonObject& payload) {
            JsonObject action = payload.createNestedArray("commands").createNestedObject();
            action["command"] = "task";
            JsonObject res = action.createNestedArray("actions").createNestedObject();
            res["action"] = "status_response";
            JsonObject p = res.createNestedObject("params");
            p["id"] = taskId;
            p["status"] = status;
          });
        }
      }
    }
  });

  Serial.println("Ready to receive tasks.");
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
* [`hyperwisor.com`](https://www.hyperwisor.com/)
* [`WiFi.h`](https://www.arduino.cc/en/Reference/WiFi)
* [`ArduinoJson`](https://arduinojson.org/)
* [`nikolaindustry-realtime`](https://github.com/your-org/nikolaindustry-realtime)
* [`Preferences`](https://www.arduino.cc/en/Reference/Preferences)
* [`Update`](https://www.arduino.cc/en/Reference/Update)
* [`DNSServer`](https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/src/DNSServer.h)

Install all via Arduino Library Manager.

---

# ✍️ Extend the Library
You can add:
### Custom command actions
### I2C/RS485 control routines
### Sensor integrations (DHT, BMP280, etc.)
### Device-side condition/logic evaluation
### Advanced scheduling or data logging
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


