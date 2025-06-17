#include "HyperTaskManager.h"

void HyperTaskManager::begin() {}

void HyperTaskManager::loop()
{
  updateBlinkTasks();
  updateFadeTasks();
  updatePulseTasks();
  updateToggleTasks();
  updateDelayTasks();
  updateIntervalTasks();
  updateRampTasks();
  updatePWMSweepTasks();
  updateDebounceTasks();
  updateSequenceTasks();
  updateTimeoutRestoreTasks();
}

void HyperTaskManager::addBlink(int pin, int onDuration, int offDuration, int repeat, const String &taskId, bool immunity)
{
  for (int i = 0; i < MAX_TASKS; i++)
  {
    if (!blinkTasks[i].active)
    {
      blinkTasks[i] = {pin, onDuration, offDuration, repeat, 0, millis(), false, true, taskId};
      pinMode(pin, OUTPUT);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["on"] = onDuration;
      params["off"] = offDuration;
      params["repeat"] = repeat;
      saveTaskDefinition(taskId, "blink", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addFade(int pin, int startBrightness, int endBrightness, int duration, const String &taskId, bool immunity)
{
  for (int i = 0; i < MAX_TASKS; i++)
  {
    if (!fadeTasks[i].active)
    {
      fadeTasks[i] = {pin, startBrightness, endBrightness, duration, millis(), true, taskId};
      pinMode(pin, OUTPUT);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["start"] = startBrightness;
      params["end"] = endBrightness;
      params["duration"] = duration;
      saveTaskDefinition(taskId, "fade", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addPulse(int pin, int duration, int state, const String &taskId, bool immunity)
{
  for (int i = 0; i < MAX_TASKS; i++)
  {
    if (!pulseTasks[i].active)
    {
      pulseTasks[i] = {pin, duration, state, millis(), true, taskId};
      pinMode(pin, OUTPUT);
      digitalWrite(pin, state);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["duration"] = duration;
      params["state"] = state;
      saveTaskDefinition(taskId, "pulse", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addToggle(int pin, int interval, const String &taskId, bool immunity)
{
  for (auto &t : toggleTasks)
  {
    if (!t.active)
    {
      t = {pin, interval, millis(), false, true, taskId};
      pinMode(pin, OUTPUT);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["interval"] = interval;
      saveTaskDefinition(taskId, "toggle", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addDelay(int pin, int delayTime, int targetState, const String &taskId, bool immunity)
{
  for (auto &t : delayTasks)
  {
    if (!t.active)
    {
      t = {pin, delayTime, targetState, millis(), true, taskId};
      pinMode(pin, OUTPUT);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["delay"] = delayTime;
      params["state"] = targetState;
      saveTaskDefinition(taskId, "delay", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addInterval(int pin, int interval, std::function<void(int)> callback, const String &taskId, bool immunity)
{
  for (auto &t : intervalTasks)
  {
    if (!t.active)
    {
      t = IntervalTask(pin, interval, callback, millis(), true, taskId);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["interval"] = interval;
      saveTaskDefinition(taskId, "interval", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addRamp(int pin, int startVal, int endVal, int duration, const String &taskId, bool immunity)
{
  for (auto &t : rampTasks)
  {
    if (!t.active)
    {
      t = {pin, startVal, endVal, duration, millis(), true, taskId};
      pinMode(pin, OUTPUT);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["start"] = startVal;
      params["end"] = endVal;
      params["duration"] = duration;
      saveTaskDefinition(taskId, "ramp", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addPWMSweep(int pin, int minPWM, int maxPWM, int step, int interval, const String &taskId, bool immunity)
{
  for (auto &t : pwmSweepTasks)
  {
    if (!t.active)
    {
      t = {pin, minPWM, maxPWM, step, true, millis(), interval, minPWM, true, taskId};
      pinMode(pin, OUTPUT);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["minPWM"] = minPWM;
      params["maxPWM"] = maxPWM;
      params["step"] = step;
      params["interval"] = interval;
      saveTaskDefinition(taskId, "pwmsweep", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addDebounce(int pin, int debounceDelay, std::function<void(int, int)> onChange, const String &taskId, bool immunity)
{
  for (auto &t : debounceTasks)
  {
    if (!t.active)
    {
      pinMode(pin, INPUT);
      t = DebounceTask(pin, digitalRead(pin), millis(), debounceDelay, onChange, true, taskId);

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["debounce"] = debounceDelay;
      saveTaskDefinition(taskId, "debounce", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addSequence(int pin, int *sequence, int *durations, int length, const String &taskId, bool immunity)
{
  for (auto &t : sequenceTasks)
  {
    if (!t.active)
    {
      // Allocate memory and copy sequence data
      int *sequenceCopy = new int[length];
      int *durationCopy = new int[length];
      for (int i = 0; i < length; i++)
      {
        sequenceCopy[i] = sequence[i];
        durationCopy[i] = durations[i];
      }

      t = {pin, sequenceCopy, durationCopy, length, 0, millis(), true, taskId};
      pinMode(pin, OUTPUT);
      digitalWrite(pin, sequenceCopy[0]);

      // Save task definition
      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      JsonArray seqArr = paramDoc.createNestedArray("sequence");
      JsonArray durArr = paramDoc.createNestedArray("timings");
      for (int i = 0; i < length; i++)
      {
        seqArr.add(sequenceCopy[i]);
        durArr.add(durationCopy[i]);
      }
      params["pin"] = pin;
      saveTaskDefinition(taskId, "sequence", params, immunity);

      return;
    }
  }
}

void HyperTaskManager::addTimeoutRestore(int pin, int tempState, int duration, const String &taskId, bool immunity)
{
  for (auto &t : timeoutTasks)
  {
    if (!t.active)
    {
      pinMode(pin, OUTPUT);
      int originalState = digitalRead(pin);
      digitalWrite(pin, tempState);
      t = {pin, tempState, originalState, millis(), duration, true, taskId};

      StaticJsonDocument<256> paramDoc;
      JsonObject params = paramDoc.to<JsonObject>();
      params["pin"] = pin;
      params["state"] = tempState;
      params["timeout"] = duration;
      saveTaskDefinition(taskId, "timeout_restore", params, immunity);

      return;
    }
  }
}

// ================= Update Loop Functions =================

void HyperTaskManager::updateBlinkTasks()
{
  unsigned long now = millis();
  for (int i = 0; i < MAX_TASKS; i++)
  {
    if (blinkTasks[i].active)
    {
      BlinkTask &t = blinkTasks[i];
      if (t.state && now - t.lastToggleTime >= t.onDuration)
      {
        digitalWrite(t.pin, LOW);
        t.state = false;
        t.lastToggleTime = now;
      }
      else if (!t.state && now - t.lastToggleTime >= t.offDuration)
      {
        if (t.count < t.repeat)
        {
          digitalWrite(t.pin, HIGH);
          t.state = true;
          t.lastToggleTime = now;
          t.count++;
        }
        else
        {
          t.active = false;
        }
      }
    }
  }
}

void HyperTaskManager::updateFadeTasks()
{
  unsigned long now = millis();
  for (int i = 0; i < MAX_TASKS; i++)
  {
    if (fadeTasks[i].active)
    {
      FadeTask &t = fadeTasks[i];
      int elapsed = now - t.startTime;
      if (elapsed >= t.duration)
      {
        analogWrite(t.pin, t.endBrightness);
        t.active = false;
      }
      else
      {
        float progress = (float)elapsed / t.duration;
        int value = t.startBrightness + progress * (t.endBrightness - t.startBrightness);
        analogWrite(t.pin, value);
      }
    }
  }
}

void HyperTaskManager::updatePulseTasks()
{
  unsigned long now = millis();
  for (int i = 0; i < MAX_TASKS; i++)
  {
    if (pulseTasks[i].active)
    {
      PulseTask &t = pulseTasks[i];
      if (now - t.startTime >= t.duration)
      {
        digitalWrite(t.pin, !t.initialState);
        t.active = false;
      }
    }
  }
}

void HyperTaskManager::updateToggleTasks()
{
  unsigned long now = millis();
  for (auto &t : toggleTasks)
  {
    if (t.active && now - t.lastToggleTime >= t.interval)
    {
      t.state = !t.state;
      digitalWrite(t.pin, t.state);
      t.lastToggleTime = now;
    }
  }
}

void HyperTaskManager::updateDelayTasks()
{
  unsigned long now = millis();
  for (auto &t : delayTasks)
  {
    if (t.active && now - t.startTime >= t.delayTime)
    {
      digitalWrite(t.pin, t.targetState);
      t.active = false;
    }
  }
}

void HyperTaskManager::updateIntervalTasks()
{
  unsigned long now = millis();
  for (auto &t : intervalTasks)
  {
    if (t.active && now - t.lastCall >= t.interval)
    {
      t.callback(t.pin);
      t.lastCall = now;
    }
  }
}

void HyperTaskManager::updateRampTasks()
{
  unsigned long now = millis();
  for (auto &t : rampTasks)
  {
    if (t.active)
    {
      int elapsed = now - t.startTime;
      if (elapsed >= t.duration)
      {
        analogWrite(t.pin, t.endVal);
        t.active = false;
      }
      else
      {
        float p = (float)elapsed / t.duration;
        int val = t.startVal + p * (t.endVal - t.startVal);
        analogWrite(t.pin, val);
      }
    }
  }
}

void HyperTaskManager::updatePWMSweepTasks()
{
  unsigned long now = millis();
  for (auto &t : pwmSweepTasks)
  {
    if (t.active && now - t.lastStepTime >= t.interval)
    {
      analogWrite(t.pin, t.value);
      t.value += t.increasing ? t.step : -t.step;
      if (t.value >= t.maxPWM || t.value <= t.minPWM)
        t.increasing = !t.increasing;
      t.lastStepTime = now;
    }
  }
}

void HyperTaskManager::updateDebounceTasks()
{
  unsigned long now = millis();
  for (auto &t : debounceTasks)
  {
    if (t.active)
    {
      int current = digitalRead(t.pin);
      if (current != t.stableState && now - t.lastChange > t.debounceDelay)
      {
        t.stableState = current;
        t.lastChange = now;
        t.onChange(t.pin, current);
      }
    }
  }
}

void HyperTaskManager::updateSequenceTasks()
{
  unsigned long now = millis();
  for (auto &t : sequenceTasks)
  {
    if (t.active && now - t.lastChange >= t.durations[t.index])
    {
      t.index++;
      if (t.index < t.length)
      {
        digitalWrite(t.pin, t.sequence[t.index]);
        t.lastChange = now;
      }
      else
      {
        t.active = false;
      }
    }
  }
}

void HyperTaskManager::updateTimeoutRestoreTasks()
{
  unsigned long now = millis();
  for (auto &t : timeoutTasks)
  {
    if (t.active && now - t.startTime >= t.duration)
    {
      digitalWrite(t.pin, t.originalState);
      t.active = false;
    }
  }
}

// ============ Task Cancel and Status by ID ==============

bool HyperTaskManager::cancelTaskById(const String &taskId)
{
  for (int i = 0; i < MAX_TASKS; i++)
  {
    if (blinkTasks[i].active && blinkTasks[i].id == taskId)
    {
      blinkTasks[i].active = false;
      return true;
    }
    if (fadeTasks[i].active && fadeTasks[i].id == taskId)
    {
      fadeTasks[i].active = false;
      return true;
    }
    if (pulseTasks[i].active && pulseTasks[i].id == taskId)
    {
      pulseTasks[i].active = false;
      return true;
    }
    if (toggleTasks[i].active && toggleTasks[i].id == taskId)
    {
      toggleTasks[i].active = false;
      return true;
    }
    if (delayTasks[i].active && delayTasks[i].id == taskId)
    {
      delayTasks[i].active = false;
      return true;
    }
    if (intervalTasks[i].active && intervalTasks[i].id == taskId)
    {
      intervalTasks[i].active = false;
      return true;
    }
    if (rampTasks[i].active && rampTasks[i].id == taskId)
    {
      rampTasks[i].active = false;
      return true;
    }
    if (pwmSweepTasks[i].active && pwmSweepTasks[i].id == taskId)
    {
      pwmSweepTasks[i].active = false;
      return true;
    }
    if (debounceTasks[i].active && debounceTasks[i].id == taskId)
    {
      debounceTasks[i].active = false;
      return true;
    }
    if (sequenceTasks[i].active && sequenceTasks[i].id == taskId)
    {
      // ⚠️ Free dynamically allocated memory
      delete[] sequenceTasks[i].sequence;
      delete[] sequenceTasks[i].durations;
      sequenceTasks[i].active = false;
      return true;
    }
    if (timeoutTasks[i].active && timeoutTasks[i].id == taskId)
    {
      timeoutTasks[i].active = false;
      return true;
    }
  }

  return false;
}

String HyperTaskManager::getTaskStatusById(const String &taskId)
{
  for (int i = 0; i < MAX_TASKS; i++)
  {
    if (blinkTasks[i].id == taskId)
      return blinkTasks[i].active ? "active" : "completed";
    if (fadeTasks[i].id == taskId)
      return fadeTasks[i].active ? "active" : "completed";
    if (pulseTasks[i].id == taskId)
      return pulseTasks[i].active ? "active" : "completed";
    if (toggleTasks[i].id == taskId)
      return toggleTasks[i].active ? "active" : "completed";
    if (delayTasks[i].id == taskId)
      return delayTasks[i].active ? "active" : "completed";
    if (intervalTasks[i].id == taskId)
      return intervalTasks[i].active ? "active" : "completed";
    if (rampTasks[i].id == taskId)
      return rampTasks[i].active ? "active" : "completed";
    if (pwmSweepTasks[i].id == taskId)
      return pwmSweepTasks[i].active ? "active" : "completed";
    if (debounceTasks[i].id == taskId)
      return debounceTasks[i].active ? "active" : "completed";
    if (sequenceTasks[i].id == taskId)
      return sequenceTasks[i].active ? "active" : "completed";
    if (timeoutTasks[i].id == taskId)
      return timeoutTasks[i].active ? "active" : "completed";
  }
  return "not_found";
}

void HyperTaskManager::saveTaskDefinition(const String &id, const String &type, JsonObject params, bool immunity)
{
  StaticJsonDocument<512> doc;
  doc["id"] = id;
  doc["type"] = type;
  doc["immunity"] = immunity;
  doc["params"] = params;

  String key = "task_" + id;
  String jsonStr;
  serializeJson(doc, jsonStr);

  taskpreferences.begin("tasks", false);
  taskpreferences.putString(key.c_str(), jsonStr);
  taskpreferences.end();

  Serial.println("Saved task: " + key + " -> " + jsonStr);
}

void HyperTaskManager::restoreAllTasks()
{
  taskpreferences.begin("tasks", true);
  int count = taskpreferences.getUInt("taskCount", 0);
  for (int i = 0; i < count; i++)
  {
    String key = "task_" + String(i);
    if (taskpreferences.isKey(key.c_str()))
      continue;

    String jsonStr = taskpreferences.getString(key.c_str());
    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, jsonStr);
    if (err)
      continue;

    String id = doc["id"];
    String type = doc["type"];
    bool immunity = doc["immunity"];
    JsonObject params = doc["params"];

    if (!immunity)
      continue;

    int pin = params["pin"] | -1;

    if (type == "fade")
    {
      addFade(pin, params["start"], params["end"], params["duration"], id, immunity);
    }
    else if (type == "blink")
    {
      addBlink(pin, params["on"], params["off"], params["repeat"], id, immunity);
    }
    else if (type == "pulse")
    {
      addPulse(pin, params["duration"], params["state"], id, immunity);
    }
    else if (type == "toggle")
    {
      addToggle(pin, params["interval"], id, immunity);
    }
    else if (type == "delay")
    {
      addDelay(pin, params["delay"], params["state"], id, immunity);
    }
    else if (type == "interval")
    {
      addInterval(pin, params["interval"], [](int p) {}, id, immunity);
    }
    else if (type == "ramp")
    {
      addRamp(pin, params["start"], params["end"], params["duration"], id, immunity);
    }
    else if (type == "pwmsweep")
    {
      addPWMSweep(pin, params["start"], params["end"], params["step"], params["delay"], id, immunity);
    }
    else if (type == "debounce")
    {
      addDebounce(pin, params["debounce"], [](int, int) {}, id, immunity);
    }
    else if (type == "sequence")
    {
      JsonArray seq = params["sequence"];
      JsonArray times = params["timings"];
      int len = seq.size();
      int *sequenceArray = new int[len];
      int *timingArray = new int[len];
      for (int i = 0; i < len; i++)
      {
        sequenceArray[i] = seq[i];
        timingArray[i] = times[i];
      }
      addSequence(pin, sequenceArray, timingArray, len, id, immunity);
      delete[] sequenceArray;
      delete[] timingArray;
    }
    else if (type == "timeout_restore")
    {
      addTimeoutRestore(pin, params["state"], params["timeout"], id, immunity);
    }
  }
  taskpreferences.end();
}

void HyperTaskManager::clearAllSavedTasks()
{
  taskpreferences.begin("tasks", false);
  taskpreferences.clear();
  taskpreferences.end();
}
