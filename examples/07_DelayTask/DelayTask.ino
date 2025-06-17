#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  // Set pin 2 HIGH after a 2000ms delay
  hyper.getTaskManager().addDelay(2, 2000, true, "delay1", false);
}

void loop() {
  hyper.loop();
}
