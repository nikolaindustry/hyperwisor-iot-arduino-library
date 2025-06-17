#include "HyperTaskManager.h"

void HyperTaskManager::begin() {
  // Optional: initialize arrays or state if needed
}

void HyperTaskManager::loop() {
  updateBlinkTasks();
  updateFadeTasks();
  updatePulseTasks();
}

void HyperTaskManager::addBlink(int pin, int onDuration, int offDuration, int repeat) {
  for (int i = 0; i < MAX_TASKS; i++) {
    if (!blinkTasks[i].active) {
      blinkTasks[i] = { pin, onDuration, offDuration, repeat, 0, millis(), false, true };
      pinMode(pin, OUTPUT);
      return;
    }
  }
}

void HyperTaskManager::addFade(int pin, int startBrightness, int endBrightness, int duration) {
  for (int i = 0; i < MAX_TASKS; i++) {
    if (!fadeTasks[i].active) {
      fadeTasks[i] = { pin, startBrightness, endBrightness, duration, millis(), true };
      pinMode(pin, OUTPUT);
      return;
    }
  }
}

void HyperTaskManager::addPulse(int pin, int duration, int state) {
  for (int i = 0; i < MAX_TASKS; i++) {
    if (!pulseTasks[i].active) {
      pulseTasks[i] = { pin, duration, state, millis(), true };
      pinMode(pin, OUTPUT);
      digitalWrite(pin, state);
      return;
    }
  }
}

void HyperTaskManager::updateBlinkTasks() {
  unsigned long now = millis();
  for (int i = 0; i < MAX_TASKS; i++) {
    if (blinkTasks[i].active) {
      BlinkTask& t = blinkTasks[i];
      if (t.state && now - t.lastToggleTime >= t.onDuration) {
        digitalWrite(t.pin, LOW);
        t.state = false;
        t.lastToggleTime = now;
      } else if (!t.state && now - t.lastToggleTime >= t.offDuration) {
        if (t.count < t.repeat) {
          digitalWrite(t.pin, HIGH);
          t.state = true;
          t.lastToggleTime = now;
          t.count++;
        } else {
          t.active = false;
        }
      }
    }
  }
}

void HyperTaskManager::updateFadeTasks() {
  unsigned long now = millis();
  for (int i = 0; i < MAX_TASKS; i++) {
    if (fadeTasks[i].active) {
      FadeTask& t = fadeTasks[i];
      int elapsed = now - t.startTime;
      if (elapsed >= t.duration) {
        analogWrite(t.pin, t.endBrightness);
        t.active = false;
      } else {
        float progress = (float)elapsed / t.duration;
        int value = t.startBrightness + progress * (t.endBrightness - t.startBrightness);
        analogWrite(t.pin, value);
      }
    }
  }
}

void HyperTaskManager::updatePulseTasks() {
  unsigned long now = millis();
  for (int i = 0; i < MAX_TASKS; i++) {
    if (pulseTasks[i].active) {
      PulseTask& t = pulseTasks[i];
      if (now - t.startTime >= t.duration) {
        digitalWrite(t.pin, !t.initialState);
        t.active = false;
      }
    }
  }
}
