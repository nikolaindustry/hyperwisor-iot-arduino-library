#include <hyperwisor-iot.h>

String from = "";
HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  hyper.restoreAllGPIOStates();
  hyper.getTaskManager().restoreAllTasks();

  hyper.setUserCommandHandler([](JsonObject& msg) {
    if (msg.containsKey("from")) {
      from = msg["from"].as<String>();
      Serial.println("Message from: " + from);
    }

    if (!msg.containsKey("payload")) return;
    JsonObject payload = msg["payload"];
    JsonArray commands = payload["commands"];

    for (JsonObject commandObj : commands) {
      if (strcmp(commandObj["command"], "TASK") != 0) continue;

      JsonArray actions = commandObj["actions"];
      for (JsonObject actionObj : actions) {
        const char* action = actionObj["action"];
        JsonObject params = actionObj["params"];
        int pin = params["pin"] | -1;
        String taskId = actionObj["id"] | "";
        bool immunity = params["immunity"] | false;

        if (strcmp(action, "blink") == 0) {
          hyper.getTaskManager().addBlink(pin, params["on"], params["off"], params["repeat"], taskId, immunity);

        } else if (strcmp(action, "fade") == 0) {
          hyper.getTaskManager().addFade(pin, params["start"], params["end"], params["duration"], taskId, immunity);

        } else if (strcmp(action, "pulse") == 0) {
          hyper.getTaskManager().addPulse(pin, params["duration"], params["state"], taskId, immunity);

        } else if (strcmp(action, "toggle") == 0) {
          hyper.getTaskManager().addToggle(pin, params["interval"], taskId, immunity);

        } else if (strcmp(action, "delay") == 0) {
          hyper.getTaskManager().addDelay(pin, params["delay"], params["state"], taskId, immunity);

        } else if (strcmp(action, "interval") == 0) {
          hyper.getTaskManager().addInterval(pin, params["interval"], [](int pin) {
            Serial.printf("Interval callback on pin %d\n", pin);
          }, taskId, immunity);

        } else if (strcmp(action, "ramp") == 0) {
          hyper.getTaskManager().addRamp(pin, params["start"], params["end"], params["duration"], taskId, immunity);

        } else if (strcmp(action, "pwmsweep") == 0) {
          hyper.getTaskManager().addPWMSweep(pin, params["start"], params["end"], params["step"], params["delay"], taskId, immunity);

        } else if (strcmp(action, "debounce") == 0) {
          hyper.getTaskManager().addDebounce(pin, params["debounce"], [](int pin, int state) {
            Serial.printf("Debounced pin %d state %d\n", pin, state);
          }, taskId, immunity);

        } else if (strcmp(action, "sequence") == 0) {
          JsonArray seq = params["sequence"];
          JsonArray times = params["timings"];
          int len = seq.size();
          int* sequenceArray = new int[len];
          int* timingArray = new int[len];
          for (int i = 0; i < len; i++) {
            sequenceArray[i] = seq[i].as<int>();
            timingArray[i] = times[i].as<int>();
          }
          hyper.getTaskManager().addSequence(pin, sequenceArray, timingArray, len, taskId, immunity);
          delete[] sequenceArray;
          delete[] timingArray;

        } else if (strcmp(action, "timeout_restore") == 0) {
          hyper.getTaskManager().addTimeoutRestore(pin, params["state"], params["timeout"], taskId, immunity);

        } else if (strcmp(action, "cancel") == 0) {
          bool result = hyper.getTaskManager().cancelTaskById(taskId);
          Serial.printf("Cancel task %s: %s\n", taskId.c_str(), result ? "Success" : "Failed");

          hyper.sendTo(from, [taskId, result](JsonObject& payload) {
            JsonObject action = payload.createNestedArray("commands").createNestedObject();
            action["command"] = "task";
            JsonObject cancel = action.createNestedArray("actions").createNestedObject();
            cancel["action"] = "cancel_response";
            JsonObject p = cancel.createNestedObject("params");
            p["id"] = taskId;
            p["success"] = result;
          });

        } else if (strcmp(action, "status") == 0) {
          String status = hyper.getTaskManager().getTaskStatusById(taskId);
          Serial.printf("Status of task %s: %s\n", taskId.c_str(), status.c_str());

          hyper.sendTo(from, [taskId, status](JsonObject& payload) {
            JsonObject action = payload.createNestedArray("commands").createNestedObject();
            action["command"] = "task";
            JsonObject res = action.createNestedArray("actions").createNestedObject();
            res["action"] = "status_response";
            JsonObject p = res.createNestedObject("params");
            p["id"] = taskId;
            p["status"] = status;
          });
        }
      }
    }
  });

  Serial.println("Ready to receive tasks.");
}

void loop() {
  hyper.loop();
}
