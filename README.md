# ESP32 Home Alarm System

An IoT-enabled home alarm system built with the ESP32 microcontroller. This project provides a robust and customizable security solution for your home, featuring remote control capabilities and real-time monitoring through the ThingsBoard platform.

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)
![Language: C++](https://img.shields.io/badge/Language-C++-brightgreen.svg)

## Features

- **Remote Control**: Arm and disarm the system using dedicated push buttons on the device or via a remote control.
- **Visual Status Indicators**:
  - An RGB LED clearly shows the system's state (e.g., green for armed, blue for disarmed, flashing red for alarm).
  - Two separate LEDs indicate the connection status to WiFi and the ThingsBoard server.
- **Audible Alarms**: A buzzer provides a loud, intermittent sound when the alarm is triggered.
- **Magnetic Sensor**: Monitors doors or windows. The alarm triggers if a sensor is tripped while the system is armed.
- **IoT Integration**: Connects to WiFi and sends telemetry data to a ThingsBoard instance for remote monitoring and control.
- **Real-time Telemetry**: Periodically sends the system's status (`systemActive`, `alarmActive`) to ThingsBoard.
- **Remote Commands**: Receives RPC (Remote Procedure Call) commands from ThingsBoard to arm or disarm the system remotely.

## Hardware Requirements

- ESP32 Development Board
- Magnetic Door/Window Sensor (Reed Switch)
- Active Buzzer
- RGB LED (or 3 separate LEDs)
- Push Buttons (x5)
  - 3 for the main unit (Arm, Disarm, Connect)
  - 2 for the remote control (Arm, Disarm)
- Breadboard and Jumper Wires
- Power Supply for the ESP32

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32 Board Manager for Arduino IDE
- [PubSubClient Library](https://github.com/knolleary/pubsubclient) by Nick O'Leary

## Configuration

1.  **Clone or download** this repository.
2.  Open the `.ino` file in the Arduino IDE.
3.  Update the following constants at the beginning of the sketch with your own credentials:
    ```cpp
    const char* WIFI_SSID = "YOUR_WIFI_SSID";
    const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
    const char* TB_SERVER = "YOUR_THINGSBOARD_SERVER"; // e.g., "mqtt.thingsboard.cloud"
    const int   TB_PORT   = 1883;
    const char* TB_TOKEN  = "YOUR_DEVICE_ACCESS_TOKEN";
    ```
4.  Upload the sketch to your ESP32 board.

## How It Works

1.  **Initialization**: Upon startup, the system initializes all pins and enters a disarmed state (blue LED).
2.  **Connection**: Press the "Connect" button to establish a connection to your WiFi network and then to the ThingsBoard MQTT broker. The status LEDs will turn on upon a successful connection.
3.  **Arming/Disarming**:
    - Press the "Active" button (on the device or remote) to arm the system. The RGB LED will turn solid green.
    - Press the "Inactive" button to disarm the system. The RGB LED will turn solid blue.
4.  **Monitoring**: When armed, the system continuously monitors the state of the magnetic sensor.
5.  **Alarm Trigger**: If the magnetic sensor detects an opening (i.e., the circuit is broken) while the system is armed, the alarm is triggered.
6.  **Alarm State**: In the alarm state, the RGB LED flashes red, and the buzzer emits an intermittent sound. This state continues until the system is disarmed.
7.  **IoT Communication**:
    - The ESP32 periodically sends telemetry data (`systemActive`, `alarmActive`) to your ThingsBoard dashboard.
    - You can also send RPC commands from your ThingsBoard dashboard to remotely arm or disarm the system.

## Circuit Diagram

A Fritzing diagram or a clear schematic of the connections would be highly beneficial here. Below is a text-based description of the connections based on the provided code:

- **RGB LED**:
  - `LED_R` (Pin 25) -> Red leg of LED
  - `LED_G` (Pin 26) -> Green leg of LED
  - `LED_B` (Pin 27) -> Blue leg of LED
- **Buzzer**:
  - `BUZZER_PIN` (Pin 32) -> Buzzer's positive (+) terminal
- **Magnetic Sensor**:
  - `MAG_SENSOR_PIN` (Pin 14) -> One terminal of the sensor (other to GND). Uses `INPUT_PULLUP`.
- **Buttons (Main Unit)**:
  - `BTN_ACTIVE_PIN` (Pin 13) -> Arm button (other to GND). Uses `INPUT_PULLUP`.
  - `BTN_INACTIVE_PIN` (Pin 12) -> Disarm button (other to GND). Uses `INPUT_PULLUP`.
  - `BTN_CONNECT` (Pin 5) -> Connect button (other to GND). Uses `INPUT_PULLUP`.
- **Buttons (Remote Control)**:
  - `BTN_ACTIVE_PIN_RE` (Pin 2) -> Arm button on remote.
  - `BTN_INACTIVE_PIN_RE` (Pin 15) -> Disarm button on remote.
- **Status LEDs**:
  - `LED_WIFI` (Pin 33) -> WiFi Status LED
  - `LED_TB` (Pin 4) -> ThingsBoard Status LED

## Contributing

Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` for more information.

## Author

**Mohsen Ziadi** 