#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  hyper.getTaskManager().addTimeoutRestore(2, true, 5000, "timeout1", false);
}

void loop() {
  hyper.loop();
}
