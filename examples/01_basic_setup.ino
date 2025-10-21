#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

String deviceId = "";
String userId = "";
String msgfrom = "";

void setup() {
  Serial.begin(115200);

  hyper.setApiKeys("mk_4a9c4f312...", "msk_b78602...");

  hyper.setTimezone("IST");

  hyper.begin();
  deviceId = hyper.getDeviceId();
  userId = hyper.getUserId();


  hyper.setUserCommandHandler([](JsonObject& msg) {
    if (!msg.containsKey("payload")) return;
    msgfrom = msg["from"].as<String>();
    Serial.println(msgfrom);

    JsonObject payload = msg["payload"];

    // Get "get_ui" command
    String command = payload["command"].as<String>();
    Serial.println(command);
    if (command == "get_ui") {
      //add update dashbaord logic here
    }


    // Get "Operate" command
    JsonObject operate = hyper.findCommand(payload, "Operate");
    if (operate.isNull()) return;
    Serial.println("Operate command found");

    // // Lock action
    // JsonObject lockParams = hyper.findParams(payload, "Operate", "Lock");
    // if (!lockParams.isNull()) {
    //   String gpio = lockParams["gpio"] | "N/A";
    //   String status = lockParams["status"] | "N/A";
    // }

    // // Unlock action
    // JsonObject unlockParams = hyper.findParams(payload, "Operate", "Unlock");
    // if (!unlockParams.isNull()) {
    //   String gpio = unlockParams["gpio"] | "N/A";
    //   String status = unlockParams["status"] | "N/A";
    // }
  });
}

void loop() {
  hyper.loop();
  delay(50);
}
