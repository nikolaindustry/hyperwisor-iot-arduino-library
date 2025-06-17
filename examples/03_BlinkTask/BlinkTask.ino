#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  // Blink pin 2, ON for 500ms, OFF for 500ms, repeat 10 times
  hyper.getTaskManager().addBlink(2, 500, 500, 10, "blink1", false);
}

void loop() {
  hyper.loop();
}
