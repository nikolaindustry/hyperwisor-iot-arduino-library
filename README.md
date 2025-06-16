# hyperwisor-iot-arduino-library

**HyperwisorIOT** is an abstraction layer for ESP32 devices that simplifies WebSocket communication, Wi-Fi management, and IoT command execution. It is built on top of the [`nikolaindustry-realtime`]library, and is designed to make your IoT devices plug-and-play for real-time applications.

---

## 📦 Features

- 🚀 Automatic Wi-Fi connection
- 🌐 Seamless integration with Nikola Industry Realtime WebSocket backend
- 📩 Built-in JSON command handler (extensible)
- 🔁 Background loop function for WebSocket keep-alive
- 🧩 Designed for extensibility — easily add logic for relays, I2C, RS485, etc.

---

## 🔧 Installation

1. Download or clone this repository.
2. Add the folder to your Arduino `libraries/` directory.
3. Open the Arduino IDE and include the library in your sketch:
   ```cpp
   #include <hyperwisor-iot.h>
---

## 🧪 Example Usage

```cpp
#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  // 👇 Register your custom command handler
  hyper.setUserCommandHandler([](JsonObject &msg) {
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
}

void loop() {
  hyper.loop();
}

```

---

## 🧠 Internals

When you call `hypervisor.begin(...)`, it:

1. Connects to Wi-Fi using the given SSID and password.
2. Initializes a connection to the nikolaindustry-realtime server.
3. Registers a built-in message handler that deserializes JSON and prints it.
4. Provides a `.loop()` function that must be called inside your `loop()` to keep the connection alive.

---

## 📤 Receiving Commands

Once connected, the server can send messages in the following JSON format:

```json
{
  "from": "device-controller-001",
  "payload": {
    "commands": [
      {
        "command": "CUSTOM_COMMAND",
        "actions": [
          {
            "action": "BUZZER_ON",
            "params": {
              "value": "true",
              "duration_ms": 2000
            }
          },
          {
            "action": "LED_BLINK",
            "params": {
              "value": "fast",
              "count": 3
            }
          }
        ]
      }
    ]
  }
}

```

The message is automatically deserialized and passed to your command logic.

---

## 📚 Dependencies

* [WiFi.h](https://www.arduino.cc/en/Reference/WiFi)
* [ArduinoJson](https://arduinojson.org/)
* [nikolaindustry-realtime](https://github.com/your-org/nikolaindustry-realtime)

Make sure to install these via the Arduino Library Manager if not already included.

---

## ✍️ Customize Your Logic

Modify the lambda function inside `setupMessageHandler()` in `hyperwisor-iot.cpp` to add your own actions like:

* Relay control (PCF8574)
* RS485 communication
* I2C LCD displays
* OTA updates

---

## 🧾 License

This library is open-source under the MIT License.

---

## 🤝 Contribution

Feel free to submit issues or pull requests to improve the logic handling, add examples, or support new protocols!

