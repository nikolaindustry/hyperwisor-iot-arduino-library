#include <hyperwisor-iot.h>

HyperwisorIOT hyper;
String from = "";

void setup() {
  Serial.begin(115200);
  hyper.begin();

  hyper.setUserCommandHandler([](JsonObject& msg) {
    if (msg.containsKey("from")) {
      from = msg["from"].as<String>();
      Serial.println("Command from: " + from);
    }

    if (msg.containsKey("payload")) {
      Serial.println("Payload received.");
      // You can now parse task from payload as in the full example
    }
  });
}

void loop() {
  hyper.loop();
}
