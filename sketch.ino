#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------- Settings (change these) ----------
const char *WIFI_SSID = "Wokwi-GUEST";   // your Wi-Fi name (Wokwi uses this one)
const char *WIFI_PASS = "";              // your Wi-Fi password
const char *MQTT_HOST = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;
const char *MQTT_TOPIC = "gec/esp32/monitor";

#define DHT_PIN   4
#define DHT_TYPE  DHT22      // use DHT22 if you simulate in Wokwi
#define OLED_W    128
#define OLED_H    64
#define OLED_ADDR 0x3C

const unsigned long PUBLISH_MS = 5000;
// ---------------------------------------------

DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 oled(OLED_W, OLED_H, &Wire, -1);
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

unsigned long lastPublish = 0;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");
}

void connectMQTT() {
  while (!mqtt.connected()) {
    String clientId = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.print("Connecting to MQTT... ");
    if (mqtt.connect(clientId.c_str())) {
      Serial.println("ok");
    } else {
      Serial.printf("failed, rc=%d, retry in 3 s\n", mqtt.state());
      delay(3000);
    }
  }
}

void showOnOled(float t, float h, bool sent) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("ESP32 IoT Monitor");
  oled.setTextSize(2);
  oled.setCursor(0, 20);
  oled.printf("T:%.1fC", t);
  oled.setCursor(0, 42);
  oled.printf("H:%.1f%%", h);
  oled.setTextSize(1);
  oled.setCursor(100, 0);
  oled.print(sent ? "MQ" : "--");
  oled.display();
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found, check I2C wiring");
    while (true) delay(1000);
  }
  oled.clearDisplay();
  oled.display();

  connectWiFi();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  unsigned long now = millis();
  if (now - lastPublish < PUBLISH_MS) return;
  lastPublish = now;

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("DHT read failed");
    return;
  }

  char payload[64];
  snprintf(payload, sizeof(payload),
           "{\"temperature\":%.1f,\"humidity\":%.1f}", t, h);
  bool ok = mqtt.publish(MQTT_TOPIC, payload);

  Serial.printf("%s -> %s (%s)\n", MQTT_TOPIC, payload, ok ? "sent" : "failed");
  showOnOled(t, h, ok);
}
