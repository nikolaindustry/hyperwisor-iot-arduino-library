#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();
  Serial.println("Hyperwisor ready!");
}

void loop() {
  hyper.loop();
}
