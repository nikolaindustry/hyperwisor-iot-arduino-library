#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  hyper.getTaskManager().addBlink(2, 200, 200, 100, "blinkToCancel", false);

  delay(2000);
  bool result = hyper.getTaskManager().cancelTaskById("blinkToCancel");
  Serial.println(result ? "Cancelled successfully" : "Cancel failed");
}

void loop() {
  hyper.loop();
}
