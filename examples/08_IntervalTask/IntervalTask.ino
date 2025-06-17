#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  hyper.getTaskManager().addInterval(2, 3000, [](int pin) {
    Serial.printf("Interval on pin %d\n", pin);
  }, "interval1", false);
}

void loop() {
  hyper.loop();
}
