#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  hyper.getTaskManager().addPWMSweep(2, 0, 255, 5, 20, "pwm1", false);
}

void loop() {
  hyper.loop();
}
