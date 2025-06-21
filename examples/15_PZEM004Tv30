#include <hyperwisor-iot.h>
#include <PZEM004Tv30.h>
#include <HardwareSerial.h>


// Use UART2 (Serial2) on ESP32 with GPIO16 (RX), GPIO17 (TX)
HardwareSerial pzemSerial(2);
PZEM004Tv30 pzem(pzemSerial, 16, 17);  // Use correct constructor

String from = "";
String idd = "";
HyperwisorIOT hyper;

void setup() {
  Serial.begin(115200);
  hyper.begin();

  //  hyper.restoreAllGPIOStates();
  //  hyper.getTaskManager().restoreAllTasks();
  delay(1000);
  Serial.println("Starting PZEM-004T v3.0 communication...");

  // No deviceAddress() call here
  float voltage = pzem.voltage();
  if (isnan(voltage)) {
    Serial.println("❌ ERROR: Communication failed. Check wiring and power.");
  } else {
    Serial.println("✅ Communication established.");
  }



  hyper.setUserCommandHandler([](JsonObject& msg) {
    if (msg.containsKey("from")) {
      from = msg["from"].as<String>();
      Serial.println("Message from: " + from);
    }

    if (!msg.containsKey("payload")) return;
    JsonObject payload = msg["payload"];

    // Extract configId safely
    String configId = payload.containsKey("configId") ? (const char*)payload["configId"] : "";

    if (configId.length() == 0) {
      Serial.println("[ERROR] configId not found in payload");
      return;
    }

    Serial.println("[INFO] Received configId: " + configId);
    JsonArray commands = payload["commands"];
  
      for (JsonObject commandObj : commands) {
      if (strcmp(commandObj["command"], "TASK") != 0) continue;

      JsonArray actions = commandObj["actions"];
      for (JsonObject actionObj : actions) {
        const char* action = actionObj["action"];
        JsonObject params = actionObj["params"];


        if (strcmp(action, "status") == 0) {


          float voltage = pzem.voltage();
          float current = pzem.current();
          float power = pzem.power();
          float energy = pzem.energy();
          float frequency = pzem.frequency();
          float pf = pzem.pf();

          if (isnan(voltage)) {
            Serial.println("⚠️ Lost connection to PZEM...");
          } else {
            Serial.print("Voltage: ");
            Serial.print(voltage);
            Serial.println(" V");
            Serial.print("Current: ");
            Serial.print(current);
            Serial.println(" A");
            Serial.print("Power: ");
            Serial.print(power);
            Serial.println(" W");
            Serial.print("Energy: ");
            Serial.print(energy);
            Serial.println(" Wh");
            Serial.print("Freq: ");
            Serial.print(frequency);
            Serial.println(" Hz");
            Serial.print("PF: ");
            Serial.print(pf);
            Serial.println("");
            Serial.println("------------------------------");
          }


          // hyper.sendTo(from, [&](JsonObject& payload) {
          //   payload["type"] = "sensorDataResponse";  
          //   payload["configId"] = configId;          
          //   payload["deviceId"] = idd;          

          //   JsonObject data = payload.createNestedObject("data");
          //   data["P1_voltage"] = voltage;
          //   data["P1_current"] = current;
          //   data["P1_power"] = power;
          //   data["P1_energy"] = energy;
          //   data["P1_frequency"] = frequency;
          //   data["P1_pf"] = pf;  // power factor
          // });

          if (payload.containsKey("configId")) {

            String configId = payload["configId"];

            // send power sensor data back
            hyper.sendSensorData(from, configId, {
              {"P1_voltage", voltage},
              {"P1_current", current},
              {"P1_power", power},
              {"P1_energy", energy},
              {"P1_frequency", frequency},
              {"P1_pf", pf}
            });
          }

        }
      }
    }
  });

  Serial.println("Ready to receive tasks.");
 idd = hyper.getDeviceId();
  
 
}

void loop() {
  hyper.loop();
}
