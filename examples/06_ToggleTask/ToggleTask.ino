#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  // Toggle pin 2 every 1000ms
  hyper.getTaskManager().addToggle(2, 1000, "toggle1", false);
}

void loop() {
  hyper.loop();
}
