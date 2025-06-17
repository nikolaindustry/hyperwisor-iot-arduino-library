#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  // Pulse pin 2 HIGH for 1000ms
  hyper.getTaskManager().addPulse(2, 1000, true, "pulse1", false);
}

void loop() {
  hyper.loop();
}
