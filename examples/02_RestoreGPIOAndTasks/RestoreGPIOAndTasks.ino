#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  hyper.restoreAllGPIOStates();
  hyper.getTaskManager().restoreAllTasks();
}

void loop() {
  hyper.loop();
}
