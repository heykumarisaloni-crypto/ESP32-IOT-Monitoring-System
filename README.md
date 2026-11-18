# ESP32 IoT Monitoring System

An ESP32-based IoT project that reads temperature and humidity from a DHT22 sensor, shows the values on an SSD1306 OLED display, and publishes them over MQTT every 5 seconds.

🔗 **Live simulation:** [Open in Wokwi](https://wokwi.com/projects/480515665903014913)

## Features
- Reads temperature (°C) and humidity (%) using a DHT22 sensor
- Shows live readings on a 128x64 SSD1306 OLED (I2C)
- Publishes JSON data to an MQTT broker every 5 seconds
- Auto-reconnects to Wi-Fi and MQTT if the connection drops
- OLED shows `MQ` when the publish succeeds and `--` when it fails

## Hardware / Components
| Component | Details |
|---|---|
| Microcontroller | ESP32 DevKit C v4 |
| Sensor | DHT22 (data pin → GPIO 4) |
| Display | SSD1306 OLED 128x64, I2C address 0x3C |

## Libraries Used
- WiFi.h
- PubSubClient
- Adafruit GFX
- Adafruit SSD1306
- DHT sensor library

## How It Works
1. ESP32 connects to Wi-Fi (`Wokwi-GUEST` in simulation).
2. It connects to the MQTT broker `broker.hivemq.com` on port 1883.
3. Every 5 seconds it reads the DHT22 sensor.
4. The data is published to the topic `gec/esp32/monitor` in this format:
   ```json
   {"temperature": 25.0, "humidity": 60.0}
   ```
5. The same values are shown on the OLED and the Serial Monitor.

## How to Run
1. Open the [Wokwi project](https://wokwi.com/projects/480515665903014913).
2. Click **Start Simulation**.
3. Open the Serial Monitor to see the connection status and published data.
4. To view the MQTT messages, subscribe to the topic `gec/esp32/monitor` using any MQTT client (e.g., MQTTX, HiveMQ web client).

## Configuration
Change these values in `sketch.ino`:
```cpp
const char *WIFI_SSID = "Wokwi-GUEST";
const char *WIFI_PASS = "";
const char *MQTT_HOST = "broker.hivemq.com";
const char *MQTT_TOPIC = "gec/esp32/monitor";
const unsigned long PUBLISH_MS = 5000;
```

## Note
`broker.hivemq.com` is a public broker, so anyone can see messages on the topic. Use a unique topic name or a private broker for real deployments.

## Future Improvements
- Add a web dashboard or Node-RED for live graphs
- Add threshold alerts for high temperature
- Use MQTT over TLS with authentication

## Author
Saloni Kumari
GEC Rajkot, GTU
