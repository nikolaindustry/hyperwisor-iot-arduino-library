#include <hyperwisor-iot.h>

HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  int sequence[] = {1, 0, 1, 0};
  int timings[] = {500, 500, 500, 500};
  hyper.getTaskManager().addSequence(2, sequence, timings, 4, "seq1", false);
}

void loop() {
  hyper.loop();
}
