# Advanced Implementations

<cite>
**Referenced Files in This Document**
- [SmartHomeSwitch.ino](file://examples/SmartHomeSwitch/SmartHomeSwitch.ino)
- [SensorDataLogger.ino](file://examples/SensorDataLogger/SensorDataLogger.ino)
- [ThreeDWidgetControl.ino](file://examples/ThreeDWidgetControl/ThreeDWidgetControl.ino)
- [AttitudeWidget_MPU6050.ino](file://examples/AttitudeWidget_MPU6050/AttitudeWidget_MPU6050.ino)
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp)
- [README.md](file://README.md)
- [library.properties](file://library.properties)
- [BasicSetup.ino](file://examples/BasicSetup/BasicSetup.ino)
- [CommandHandler.ino](file://examples/CommandHandler/CommandHandler.ino)
- [WidgetUpdate.ino](file://examples/WidgetUpdate/WidgetUpdate.ino)
- [Conditional_Provisioning_Example.ino](file://examples/Conditional_Provisioning_Example/Conditional_Provisioning_Example.ino)
- [i2c_PCF8574_relay.ino](file://examples/i2c_PCF8574_relay/i2c_PCF8574_relay.ino)
</cite>

## Table of Contents
1. [Introduction](#introduction)
2. [Project Structure](#project-structure)
3. [Core Components](#core-components)
4. [Architecture Overview](#architecture-overview)
5. [Detailed Component Analysis](#detailed-component-analysis)
6. [Dependency Analysis](#dependency-analysis)
7. [Performance Considerations](#performance-considerations)
8. [Troubleshooting Guide](#troubleshooting-guide)
9. [Conclusion](#conclusion)
10. [Appendices](#appendices)

## Introduction
This document presents advanced implementation examples for Hyperwisor-IOT, focusing on three complex application domains:
- SmartHomeSwitch: Sophisticated hardware control with multiple relays, physical button inputs, debouncing, power-loss resume, and bidirectional cloud-state synchronization.
- SensorDataLogger: Sensor integration patterns, data aggregation, logging strategies, and periodic data transmission to the Hyperwisor platform.
- ThreeDWidgetControl: Interactive 3D visualization techniques, transformation matrices, and dynamic model updates.
- AttitudeWidget_MPU6050: Sensor fusion, attitude calculation, and real-time 3D orientation visualization using an MPU6050 IMU.

Each example includes comprehensive code walkthroughs, hardware requirements, integration patterns, performance considerations, and advanced troubleshooting guidance. Optimization techniques for memory usage, power consumption, and network efficiency are also provided.

## Project Structure
The repository organizes examples by use case and the core library under src. The examples demonstrate practical integrations with the Hyperwisor-IOT library for real-time communication, widget updates, and sensor/actuator control.

```mermaid
graph TB
subgraph "Examples"
SHS["SmartHomeSwitch.ino"]
SDL["SensorDataLogger.ino"]
TDC["ThreeDWidgetControl.ino"]
AW["AttitudeWidget_MPU6050.ino"]
BS["BasicSetup.ino"]
CH["CommandHandler.ino"]
WU["WidgetUpdate.ino"]
CP["Conditional_Provisioning_Example.ino"]
I2C["i2c_PCF8574_relay.ino"]
end
subgraph "Core Library"
H["hyperwisor-iot.h"]
CPP["hyperwisor-iot.cpp"]
LIB["library.properties"]
README["README.md"]
end
SHS --> H
SDL --> H
TDC --> H
AW --> H
BS --> H
CH --> H
WU --> H
CP --> H
I2C --> H
H --> CPP
LIB --> H
README --> H
```

**Diagram sources**
- [SmartHomeSwitch.ino](file://examples/SmartHomeSwitch/SmartHomeSwitch.ino#L1-L355)
- [SensorDataLogger.ino](file://examples/SensorDataLogger/SensorDataLogger.ino#L1-L77)
- [ThreeDWidgetControl.ino](file://examples/ThreeDWidgetControl/ThreeDWidgetControl.ino#L1-L85)
- [AttitudeWidget_MPU6050.ino](file://examples/AttitudeWidget_MPU6050/AttitudeWidget_MPU6050.ino#L1-L95)
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h#L1-L190)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L1-L1811)
- [library.properties](file://library.properties#L1-L11)
- [README.md](file://README.md#L1-L173)

**Section sources**
- [README.md](file://README.md#L1-L173)
- [library.properties](file://library.properties#L1-L11)

## Core Components
The Hyperwisor-IOT library provides:
- Real-time communication via nikolaindustry-realtime with automatic reconnection and retry logic.
- Wi-Fi provisioning with AP mode fallback and web-based provisioning.
- Widget update APIs for numeric, string, and array values, plus specialized widgets (flight attitude, 3D models, heat maps, countdowns).
- GPIO management and persistence.
- Database operations, SMS service, authentication, and NTP-based time/date utilities.
- OTA firmware update capability with version tracking.

Key capabilities leveraged by the advanced examples:
- User command handler for custom logic and bidirectional control.
- Widget update functions for real-time dashboards.
- Sensor data logging helper for structured telemetry.
- 3D widget update with transformation matrices and material properties.
- Flight attitude widget for roll/pitch visualization.

**Section sources**
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h#L39-L187)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L46-L137)
- [README.md](file://README.md#L22-L36)

## Architecture Overview
The examples integrate with the Hyperwisor-IOT core through a consistent lifecycle:
- Initialization: begin() connects to Wi-Fi or starts AP provisioning.
- Loop: maintain real-time connection and handle incoming commands.
- Widget updates: push state to dashboards using updateWidget/update3DWidget/updateFlightAttitude.
- Sensor data: log and transmit sensor readings periodically.
- GPIO control: manage relays and persist states across power cycles.

```mermaid
sequenceDiagram
participant Dev as "Device Sketch"
participant Lib as "HyperwisorIOT"
participant RT as "Realtime Socket"
participant Hub as "Hyperwisor Platform"
Dev->>Lib : begin()
Lib->>Lib : connectToWiFi()/startAPMode()
Lib->>RT : realtime.begin(deviceId)
Lib->>Lib : setupMessageHandler()
loop Every loop()
Dev->>Lib : loop()
Lib->>RT : realtime.loop()
alt WiFi connected
RT-->>Lib : incoming messages
Lib->>Dev : userCommandCallback(msg)
else WiFi disconnected
Lib->>Lib : attempt reconnect
end
end
Dev->>Lib : updateWidget()/update3DWidget()/updateFlightAttitude()
Lib->>RT : sendJson(payload)
RT->>Hub : forward widget updates
```

**Diagram sources**
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L13-L137)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L313-L404)
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h#L75-L107)

## Detailed Component Analysis

### SmartHomeSwitch: Sophisticated Hardware Control and Bidirectional State Management
This example demonstrates:
- 7-channel relay control with physical push-button inputs.
- Debounced switch handling to prevent chatter.
- Power-loss resume using Preferences to persist relay states.
- Bidirectional control: remote commands update local relays and vice versa.
- Real-time cloud state synchronization when Wi-Fi is available.

Hardware requirements:
- ESP32 development board.
- 7-channel relay module wired to ESP32 GPIO pins.
- Momentary push buttons per relay input.
- Pull-up or pull-down resistors as needed for switches.

Implementation highlights:
- Pin definitions for relays and switches.
- State variables and debounce tracking arrays.
- Helper functions to update relay state, persist to Preferences, and send updates to the cloud.
- User command handler mapping widget IDs to relay numbers and applying ON/OFF states.
- Loop routine scanning each switch with debounce and triggering updates and cloud sync.

```mermaid
flowchart TD
Start(["Setup"]) --> InitPrefs["Open relay-storage Preferences"]
InitPrefs --> SetupRelays["Configure relay pins OUTPUT"]
SetupRelays --> RestoreStates["Load persisted relay states"]
RestoreStates --> SetupSwitches["Configure switch pins INPUT"]
SetupSwitches --> ReadInitial["Read initial switch positions"]
ReadInitial --> SetupCloud["Set user command handler"]
SetupCloud --> Begin["device.begin()"]
Begin --> Loop(["Loop"])
Loop --> ScanSW1["Read SW1, debounce, toggle R1, updatePrefs, sendToCloud"]
Loop --> ScanSW2["Repeat for SW2..SW7"]
ScanSW1 --> Loop
ScanSW2 --> Loop
```

**Diagram sources**
- [SmartHomeSwitch.ino](file://examples/SmartHomeSwitch/SmartHomeSwitch.ino#L111-L354)

Integration patterns:
- Use Preferences for non-volatile state persistence.
- Map widget IDs to internal relay indices using flexible string matching.
- Validate action names and parameters to support multiple dashboard formats.

Performance considerations:
- Debounce delay balances reliability vs. responsiveness.
- Minimal loop delay prevents excessive CPU usage.
- Cloud updates are conditional on Wi-Fi connectivity.

Advanced troubleshooting:
- Verify relay wiring polarity and use appropriate flyback diodes.
- Confirm switch pull-up/pull-down configurations.
- Ensure widget IDs match between dashboard and device logic.

**Section sources**
- [SmartHomeSwitch.ino](file://examples/SmartHomeSwitch/SmartHomeSwitch.ino#L1-L355)

### SensorDataLogger: Sensor Integration, Aggregation, and Periodic Transmission
This example showcases:
- Structured sensor data logging with a helper method.
- Periodic sampling at fixed intervals.
- Sending multiple metrics (temperature, humidity, pressure) as a single telemetry payload.

```mermaid
sequenceDiagram
participant Dev as "Device Sketch"
participant Lib as "HyperwisorIOT"
participant Hub as "Hyperwisor Platform"
Dev->>Lib : device.begin()
loop Every readingInterval
Dev->>Dev : readTemperature()/readHumidity()/readPressure()
Dev->>Lib : send_Sensor_Data_logger(targetId, configId, data)
Lib->>Hub : type=sensorDataResponse, configId, deviceId, data
end
```

**Diagram sources**
- [SensorDataLogger.ino](file://examples/SensorDataLogger/SensorDataLogger.ino#L34-L62)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L535-L549)

Integration patterns:
- Replace simulated sensor functions with actual sensor reads (I2C/SPI).
- Use a configuration ID to group related metrics in the dashboard.
- Ensure Wi-Fi connectivity before logging to avoid blocking.

Performance considerations:
- Tune readingInterval to balance battery life and data freshness.
- Aggregate multiple sensors per interval to reduce network overhead.

Advanced troubleshooting:
- Validate sensor initialization and address I2C bus conflicts.
- Monitor free heap and adjust interval if memory pressure occurs.

**Section sources**
- [SensorDataLogger.ino](file://examples/SensorDataLogger/SensorDataLogger.ino#L1-L77)
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h#L76-L76)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L535-L549)

### ThreeDWidgetControl: 3D Model Manipulation and Interactive Visualization
This example demonstrates:
- Updating multiple 3D models within a single 3D widget.
- Transformation matrices: position, rotation, scale.
- Material properties: color, metalness, roughness, opacity.
- Visibility and wireframe rendering toggles.
- Periodic updates with incremental rotation.

```mermaid
sequenceDiagram
participant Dev as "Device Sketch"
participant Lib as "HyperwisorIOT"
participant Hub as "Hyperwisor Platform"
Dev->>Lib : device.begin()
loop Every updateInterval
Dev->>Dev : build vector<ThreeDModelUpdate>
Dev->>Lib : update3DWidget(targetId, widgetId, updates)
Lib->>Hub : widgetId=3DWidget, value=[{modelId, updates}]
Dev->>Dev : increment rotationAngle
end
```

**Diagram sources**
- [ThreeDWidgetControl.ino](file://examples/ThreeDWidgetControl/ThreeDWidgetControl.ino#L34-L83)
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h#L24-L35)
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h#L107-L107)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L686-L714)

Integration patterns:
- Assign unique model IDs per 3D object.
- Use ThreeDModelUpdate structure to specify transformations and materials.
- Limit update frequency to maintain smooth visualization.

Performance considerations:
- Reduce updateInterval for complex scenes.
- Prefer wireframe mode sparingly to preserve bandwidth.
- Minimize JSON payload size by avoiding unnecessary fields.

Advanced troubleshooting:
- Verify model URLs and widget IDs in the dashboard.
- Ensure Wi-Fi stability for continuous updates.

**Section sources**
- [ThreeDWidgetControl.ino](file://examples/ThreeDWidgetControl/ThreeDWidgetControl.ino#L1-L85)
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h#L24-L35)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L686-L714)

### AttitudeWidget_MPU6050: Sensor Fusion, Attitude Calculation, and Real-Time 3D Orientation
This example integrates:
- MPU6050 accelerometer/gyroscope via I2C.
- Roll/pitch estimation from accelerometer readings.
- Real-time updates to a Flight Attitude Widget on the dashboard.

```mermaid
sequenceDiagram
participant Dev as "Device Sketch"
participant MPU as "MPU6050"
participant Lib as "HyperwisorIOT"
participant Hub as "Hyperwisor Platform"
Dev->>Lib : device.begin()
Dev->>MPU : Wire.begin(SDA,SCL), mpu.begin()
loop Every updateInterval
Dev->>MPU : getEvent()
MPU-->>Dev : acceleration, gyro, temp
Dev->>Dev : compute roll = atan2(acc_y, acc_z)
Dev->>Dev : compute pitch = atan2(-acc_x, norm)
Dev->>Lib : updateFlightAttitude(targetId, widgetId, roll, pitch)
Lib->>Hub : widgetId=AttitudeWidget, value={roll, pitch}
end
```

**Diagram sources**
- [AttitudeWidget_MPU6050.ino](file://examples/AttitudeWidget_MPU6050/AttitudeWidget_MPU6050.ino#L33-L94)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L631-L638)

Integration patterns:
- Configure MPU6050 ranges and filter bandwidth for stable readings.
- Use custom I2C pins if needed.
- Update at a consistent rate suitable for visualization.

Performance considerations:
- Keep updateInterval low (e.g., 100 ms) for responsive visualization.
- Filter sensor noise by averaging or smoothing if necessary.

Advanced troubleshooting:
- Confirm MPU6050 detection and wiring.
- Calibrate sensor placement to minimize vibration interference.

**Section sources**
- [AttitudeWidget_MPU6050.ino](file://examples/AttitudeWidget_MPU6050/AttitudeWidget_MPU6050.ino#L1-L95)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L631-L638)

## Dependency Analysis
The examples depend on the Hyperwisor-IOT library and optional external libraries. The core library manages Wi-Fi, provisioning, real-time messaging, and widget updates.

```mermaid
graph TB
SHS["SmartHomeSwitch.ino"] --> HW["hyperwisor-iot.h/cpp"]
SDL["SensorDataLogger.ino"] --> HW
TDC["ThreeDWidgetControl.ino"] --> HW
AW["AttitudeWidget_MPU6050.ino"] --> HW
CH["CommandHandler.ino"] --> HW
WU["WidgetUpdate.ino"] --> HW
CP["Conditional_Provisioning_Example.ino"] --> HW
I2C["i2c_PCF8574_relay.ino"] --> HW
HW --> ARJ["ArduinoJson"]
HW --> WS["WebSockets"]
HW --> ESP["ESP32 SDK (WiFi, HTTP, Preferences, etc.)"]
```

**Diagram sources**
- [library.properties](file://library.properties#L10-L10)
- [hyperwisor-iot.h](file://src/hyperwisor-iot.h#L11-L14)
- [README.md](file://README.md#L92-L121)

**Section sources**
- [library.properties](file://library.properties#L10-L11)
- [README.md](file://README.md#L92-L121)

## Performance Considerations
Memory usage:
- Use compact data structures (e.g., float arrays for transforms).
- Avoid frequent dynamic allocations; reuse buffers where possible.
- Prefer stack allocation for small, short-lived objects.

Power consumption:
- Implement sleep/idle modes when feasible.
- Reduce update frequency for non-critical widgets.
- Use deep sleep for periodic tasks with long intervals.

Network efficiency:
- Batch updates (e.g., multiple sensor values per payload).
- Throttle widget updates to visualization-friendly rates.
- Retry with exponential backoff for OTA and database operations.

Wi-Fi and real-time stability:
- Monitor connection status and handle reconnections gracefully.
- Use targeted updates (only changed values) to reduce traffic.
- Validate payload sizes to prevent fragmentation issues.

[No sources needed since this section provides general guidance]

## Troubleshooting Guide
Common issues and resolutions:
- AP mode stuck: The device restarts after a timeout if provisioning is not completed. Ensure the provisioning page is reachable and credentials are submitted correctly.
- Wi-Fi reconnection loops: Excessive disconnections trigger automatic retries; verify router credentials and signal strength.
- OTA failures: Inspect HTTP response codes and available flash space; ensure secure endpoint accessibility.
- Widget updates not appearing: Confirm targetId/widgetId correctness and Wi-Fi connectivity before sending updates.
- Sensor data not logged: Check interval timing and Wi-Fi availability; validate sensor initialization and I2C addresses.

Operational checks:
- Use provided basic examples to validate provisioning and connectivity.
- Employ command handler examples to inspect incoming messages and debug custom logic.
- Utilize widget update examples to confirm dashboard communication.

**Section sources**
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L116-L136)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L1417-L1503)
- [BasicSetup.ino](file://examples/BasicSetup/BasicSetup.ino#L1-L39)
- [CommandHandler.ino](file://examples/CommandHandler/CommandHandler.ino#L1-L96)
- [WidgetUpdate.ino](file://examples/WidgetUpdate/WidgetUpdate.ino#L1-L68)

## Conclusion
The Hyperwisor-IOT library enables robust, real-time IoT applications on ESP32. These advanced examples illustrate:
- Reliable hardware control with persistence and bidirectional synchronization.
- Efficient sensor logging and periodic telemetry.
- Interactive 3D visualization with precise transformation control.
- Real-time attitude visualization leveraging sensor fusion.

By following the integration patterns, performance guidelines, and troubleshooting steps outlined above, developers can build scalable, maintainable IoT solutions tailored to complex use cases.

[No sources needed since this section summarizes without analyzing specific files]

## Appendices

### Appendix A: Example Lifecycle Patterns
- Initialization: Call begin() to connect or provision; retrieve device/user IDs for dashboard targeting.
- Loop maintenance: Invoke loop() continuously to keep real-time connections alive and process incoming commands.
- Widget updates: Use appropriate update functions for the desired widget type and data format.

**Section sources**
- [BasicSetup.ino](file://examples/BasicSetup/BasicSetup.ino#L21-L38)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L13-L28)

### Appendix B: Provisioning Options
- Automatic provisioning: AP mode with web UI for first-time setup.
- Manual provisioning: Save credentials programmatically for headless deployments.
- Conditional provisioning: Combine manual defaults with AP mode fallback.

**Section sources**
- [Conditional_Provisioning_Example.ino](file://examples/Conditional_Provisioning_Example/Conditional_Provisioning_Example.ino#L22-L68)
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L141-L185)

### Appendix C: GPIO and Persistence Utilities
- Save/load GPIO states and restore on boot for power-loss resume.
- Use Preferences for small, structured data like relay states.

**Section sources**
- [hyperwisor-iot.cpp](file://src/hyperwisor-iot.cpp#L1383-L1414)
- [SmartHomeSwitch.ino](file://examples/SmartHomeSwitch/SmartHomeSwitch.ino#L118-L149)