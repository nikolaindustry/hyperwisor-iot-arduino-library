#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {

  hyper.setApiKeys("mk_4a9c4f312...", "msk_b78602...");

  hyper.setTimezone("IST");

  hyper.begin();

  hyper.setUserCommandHandler([](JsonObject& msg) {
  //   {
  //   "payload": {
  //     "commands": [
  //       {
  //         "command": "Operate",
  //         "actions": [
  //           {
  //             "action": "Lock",
  //             "params": { "gpio": "12", "status": "HIGH" }
  //           },
  //           {
  //             "action": "Unlock",
  //             "params": { "gpio": "12", "status": "LOW" }
  //           }
  //         ]
  //       }
  //     ]
  //   }
  // }
    
    if (!msg.containsKey("payload")) return;
    JsonObject payload = msg["payload"];

    // Get "Operate" command
    JsonObject operate = hyper.findCommand(payload, "Operate");
    if (operate.isNull()) return;
    Serial.println("Operate command found");

    // Lock action
    JsonObject lockParams = hyper.findParams(payload, "Operate", "Lock");
    if (!lockParams.isNull()) {
      String gpio = lockParams["gpio"] | "N/A";
      String status = lockParams["status"] | "N/A";
    }

    // Unlock action
    JsonObject unlockParams = hyper.findParams(payload, "Operate", "Unlock");
    if (!unlockParams.isNull()) {
      String gpio = unlockParams["gpio"] | "N/A";
      String status = unlockParams["status"] | "N/A";
    }
  });
}

void loop() {
  hyper.loop();
  delay(50);
}
