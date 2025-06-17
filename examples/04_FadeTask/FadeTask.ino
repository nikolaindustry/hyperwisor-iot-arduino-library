#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  hyper.getTaskManager().addFade(2, 0, 255, 2000, "fade1", false);
}

void loop() {
  hyper.loop();
}
