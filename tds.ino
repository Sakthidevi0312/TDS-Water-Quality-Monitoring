#include <WiFi.h>
#include <PubSubClient.h>

#define TDS_PIN 32         // Analog pin for TDS sensor
#define VREF 3.3           // Analog reference voltage
#define ADC_RES 4095.0     // 12-bit ADC for ESP32

// Wi-Fi credentials
const char* ssid = "Galaxy A14 5G";
const char* password = "sakthi1203";

// MQTT broker details
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttTopic = "watermonitor/tds";

// Create WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(TDS_PIN, INPUT);
  connectWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Read TDS sensor
  int adcValue = analogRead(TDS_PIN);
  float voltage = adcValue * (VREF / ADC_RES);
  float tds = (133.42 * pow(voltage, 3) - 255.86 * pow(voltage, 2) + 857.39 * voltage) * 0.5;

  // Print to Serial
  Serial.print("TDS: ");
  Serial.print(tds);
  Serial.println(" ppm");

  // Publish to MQTT
  char payload[32];
  dtostrf(tds, 6, 2, payload); // Convert float to string
  client.publish(mqttTopic, payload);

  delay(5000); // Delay between readings
}

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println(" connected.");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}


