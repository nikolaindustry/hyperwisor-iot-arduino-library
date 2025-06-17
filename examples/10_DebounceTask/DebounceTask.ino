#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  pinMode(2, INPUT);
  hyper.getTaskManager().addDebounce(2, 50, [](int pin, int state) {
    Serial.printf("Debounced input on pin %d: %d\n", pin, state);
  }, "debounce1", false);
}

void loop() {
  hyper.loop();
}
