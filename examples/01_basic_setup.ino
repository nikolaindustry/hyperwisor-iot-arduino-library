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
      //add update dashbaord logic here when user open the dashboard
    }

    // Get command
    JsonObject commandName = hyper.findCommand(payload, "commandName");
    if (commandName.isNull()) return;
    Serial.println("commandName command found");

    // Get action
    JsonObject actionParams = hyper.findAction(payload, "commandName", "actionName");
    if (!actionParams.isNull()) {
      String paramsValue = actionParams["params"]["paramsKey"] | "N/A";
    }
  });
}

void loop() {
  hyper.loop();
  delay(50);
}
