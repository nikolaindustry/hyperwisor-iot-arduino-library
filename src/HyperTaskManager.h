#ifndef HYPER_TASK_MANAGER_H
#define HYPER_TASK_MANAGER_H

#include <Arduino.h>

#define MAX_TASKS 20

struct BlinkTask {
  int pin, onDuration, offDuration, repeat, count;
  unsigned long lastToggleTime;
  bool state, active;
};

struct FadeTask {
  int pin, startBrightness, endBrightness, duration;
  unsigned long startTime;
  bool active;
};

struct PulseTask {
  int pin, duration, initialState;
  unsigned long startTime;
  bool active;
};

class HyperTaskManager {
public:
  void begin();
  void loop();

  void addBlink(int pin, int onDuration, int offDuration, int repeat);
  void addFade(int pin, int startBrightness, int endBrightness, int duration);
  void addPulse(int pin, int duration, int state);

private:
  BlinkTask blinkTasks[MAX_TASKS];
  FadeTask fadeTasks[MAX_TASKS];
  PulseTask pulseTasks[MAX_TASKS];

  void updateBlinkTasks();
  void updateFadeTasks();
  void updatePulseTasks();
};

#endif
