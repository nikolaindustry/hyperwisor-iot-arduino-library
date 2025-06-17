# HyperwisorIOT Arduino Library

The **HyperwisorIOT** library is designed to help developers build dynamic IoT logic using reusable, JSON-controllable tasks like blink, fade, pulse, toggle, debounce, etc.

---

## 📁 Examples

Explore examples below to get started:

| #  | Example                | Description                                                |
|----|------------------------|------------------------------------------------------------|
| 1  | [BasicSetup](examples/01_BasicSetup)             | Minimal setup and loop for starting HyperwisorIOT         |
| 2  | [RestoreGPIOAndTasks](examples/02_RestoreGPIOAndTasks) | Restore GPIO states and running tasks from memory         |
| 3  | [BlinkTask](examples/03_BlinkTask)               | Blink a pin with on/off time and repeat count             |
| 4  | [FadeTask](examples/04_FadeTask)                 | Fade LED using PWM                                        |
| 5  | [PulseTask](examples/05_PulseTask)               | Pulse a pin for fixed time                                |
| 6  | [ToggleTask](examples/06_ToggleTask)             | Toggle a pin at interval                                  |
| 7  | [DelayTask](examples/07_DelayTask)               | Delay turning pin on/off                                  |
| 8  | [IntervalTask](examples/08_IntervalTask)         | Perform an action at intervals                            |
| 9  | [PWMSweepTask](examples/09_PWMSweepTask)         | Sweep PWM values between two points                       |
| 10 | [DebounceTask](examples/10_DebounceTask)         | Debounce a physical button input                          |
| 11 | [SequenceTask](examples/11_SequenceTask)         | Output a sequence of HIGH/LOW on a pin                    |
| 12 | [TimeoutRestoreTask](examples/12_TimeoutRestoreTask) | Set pin state and restore after timeout                   |
| 13 | [CancelTask](examples/13_CancelTask)             | Cancel a task by ID                                       |
| 14 | [UserCommandHandler](examples/14_UserCommandHandler) | Parse and act on incoming JSON tasks                      |

---

## 📦 Installation

1. Clone this repo or download ZIP
2. Place in `Arduino/libraries/HyperwisorIOT/`
3. Open any `.ino` from `examples/` and upload to your ESP32

---

## 🧠 About Tasks

Each task has this JSON structure:
```json
{
  "command": "TASK",
  "actions": [
    {
      "action": "blink",
      "id": "blink1",
      "params": {
        "pin": 2,
        "on": 500,
        "off": 500,
        "repeat": 10
      }
    }
  ]
}
