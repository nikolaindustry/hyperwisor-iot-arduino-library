#ifndef HYPER_TASK_MANAGER_H
#define HYPER_TASK_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <vector>
#include <map> // <-- This is essential
#include <Wire.h>
#define MAX_TASKS 20

// ------------------------ STRUCTS ------------------------

struct BlinkTask
{
  int pin, onDuration, offDuration, repeat, count;
  unsigned long lastToggleTime;
  bool state, active;
  String id;
};

struct FadeTask
{
  int pin, startBrightness, endBrightness, duration;
  unsigned long startTime;
  bool active;
  String id;
};

struct PulseTask
{
  int pin, duration, initialState;
  unsigned long startTime;
  bool active;
  String id;
};

struct ToggleTask
{
  int pin;
  int interval;
  unsigned long lastToggleTime;
  bool state;
  bool active;
  String id;
};

struct DelayTask
{
  int pin;
  int delayTime;
  int targetState;
  unsigned long startTime;
  bool active;
  String id;
};

struct RampTask
{
  int pin;
  int startVal, endVal;
  int duration;
  unsigned long startTime;
  bool active;
  String id;
};

struct PWMSweepTask
{
  int pin;
  int minPWM, maxPWM;
  int step;
  bool increasing;
  unsigned long lastStepTime;
  int interval;
  int value;
  bool active;
  String id;
};

struct SequenceTask
{
  int pin;
  int *sequence;
  int *durations;
  int length;
  int index;
  unsigned long lastChange;
  bool active;
  String id;
};

struct TimeoutRestoreTask
{
  int pin;
  int tempState;
  int originalState;
  unsigned long startTime;
  int duration;
  bool active;
  String id;
};

struct IntervalTask
{
  int pin;
  int interval;
  std::function<void(int)> callback;
  unsigned long lastCall;
  bool active;
  String id;

  IntervalTask(int p, int i, std::function<void(int)> cb, unsigned long last, bool a, const String &id)
      : pin(p), interval(i), callback(cb), lastCall(last), active(a), id(id) {}

  IntervalTask() = default;
};

struct DebounceTask
{
  int pin;
  int stableState;
  unsigned long lastChange;
  int debounceDelay;
  std::function<void(int, int)> onChange;
  bool active;
  String id;

  DebounceTask(int p, int state, unsigned long change, int delay, std::function<void(int, int)> cb, bool a, const String &id)
      : pin(p), stableState(state), lastChange(change), debounceDelay(delay), onChange(cb), active(a), id(id) {}

  DebounceTask() = default;
};

struct I2CTask
{
  uint8_t sda;
  uint8_t scl;
  uint8_t deviceAddress;
  std::vector<uint8_t> writeData;
  size_t readLen;
  std::vector<uint8_t> readResult;
  bool isRead;
  String taskId;
  bool immunity;
};

// ------------------------ CLASS ------------------------

class HyperTaskManager
{
public:
  void begin();
  void loop();

  // Task adders with optional taskId
  void addBlink(int pin, int on, int off, int repeat, const String &id, bool immunity);
  void addFade(int pin, int start, int end, int duration, const String &id, bool immunity);
  void addPulse(int pin, int duration, int state, const String &id, bool immunity);
  void addToggle(int pin, int interval, const String &id, bool immunity);
  void addDelay(int pin, int delayTime, int state, const String &id, bool immunity);
  void addInterval(int pin, int interval, std::function<void(int)> callback, const String &id, bool immunity);
  void addRamp(int pin, int start, int end, int duration, const String &id, bool immunity);
  void addPWMSweep(int pin, int start, int end, int step, int delayMs, const String &id, bool immunity);
  void addDebounce(int pin, int debounceTime, std::function<void(int, int)> callback, const String &id, bool immunity);
  void addSequence(int pin, int *sequence, int *timings, int length, const String &id, bool immunity);
  void addTimeoutRestore(int pin, int state, int timeout, const String &id, bool immunity);
  void saveTaskDefinition(const String &id, const String &type, JsonObject params, bool immunity);
  void restoreAllTasks();
  void clearAllSavedTasks();

  std::vector<uint8_t> getI2CReadResult(const String &taskId);
  bool cancelI2CTaskById(const String &taskId);
  String getI2CTaskStatusById(const String &taskId);
  void addI2CTask(uint8_t sda, uint8_t scl, uint8_t address, const std::vector<uint8_t> &writeData, const String &taskId = "", bool immunity = false);
  void addI2CReadTask(uint8_t sda, uint8_t scl, uint8_t address, size_t readLen, const String &taskId = "", bool immunity = false);
  bool cancelTaskById(const String &taskId);
  String getTaskStatusById(const String &taskId);
  TwoWire *getWireInstance(uint8_t sda, uint8_t scl);
  void clearWireInstances();
  bool removeTaskFromPersistence(const String &taskId);
  std::vector<String> listAllPersistentTasks();
  String getPersistentTaskJsonById(const String &taskId);

private:
  BlinkTask blinkTasks[MAX_TASKS];
  FadeTask fadeTasks[MAX_TASKS];
  PulseTask pulseTasks[MAX_TASKS];
  ToggleTask toggleTasks[MAX_TASKS];
  DelayTask delayTasks[MAX_TASKS];
  IntervalTask intervalTasks[MAX_TASKS];
  RampTask rampTasks[MAX_TASKS];
  PWMSweepTask pwmSweepTasks[MAX_TASKS];
  DebounceTask debounceTasks[MAX_TASKS];
  SequenceTask sequenceTasks[MAX_TASKS];
  TimeoutRestoreTask timeoutTasks[MAX_TASKS];
  Preferences taskpreferences;

  // Internal loop updates
  void updateBlinkTasks();
  void updateFadeTasks();
  void updatePulseTasks();
  void updateToggleTasks();
  void updateDelayTasks();
  void updateIntervalTasks();
  void updateRampTasks();
  void updatePWMSweepTasks();
  void updateDebounceTasks();
  void updateSequenceTasks();
  void updateTimeoutRestoreTasks();
  std::vector<I2CTask> i2cTasks;
  std::map<String, std::vector<uint8_t>> i2cTaskResults;
  std::map<String, String> i2cTaskStatuses;
  std::map<String, TwoWire *> wireInstances;

  void updateI2CTasks();
};

#endif
