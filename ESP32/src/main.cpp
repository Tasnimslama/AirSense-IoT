#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// Brochage valide du projet AirSense ESP32
#define PIR_PIN 27
#define LDR_PIN 26
#define BUZZER_PIN 25
#define DARK_LEVEL LOW

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
unsigned long dernierWifi = 0;
unsigned long dernierMQTT = 0;
unsigned long dernierePublication = 0;

void maintenirReseau() {
  if (WiFi.status() != WL_CONNECTED) {
    if (dernierWifi == 0 || millis() - dernierWifi >= 10000) {
      dernierWifi = millis();
      Serial.println("[WiFi] Reconnexion...");
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    return;
  }
  if (mqtt.connected()) {
    mqtt.loop();
    return;
  }
  if (dernierMQTT == 0 || millis() - dernierMQTT >= 5000) {
    dernierMQTT = millis();
    String id = "AirSense-ESP32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    if (mqtt.connect(id.c_str())) {
      Serial.println("[MQTT] ESP32 connecte");
      dernierePublication = millis() - 2000;
    } else {
      Serial.printf("[MQTT] Echec, code=%d\n", mqtt.state());
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  dernierWifi = millis();
  mqtt.setServer(BROKER_IP, 1883);
  mqtt.setSocketTimeout(3);
  mqtt.setKeepAlive(20);
  Serial.println("AirSense ESP32 pret");
}

void loop() {
  // La logique locale du buzzer fonctionne meme sans Wi-Fi / MQTT.
  const bool mouvement = digitalRead(PIR_PIN) == HIGH;
  const bool obscurite = digitalRead(LDR_PIN) == DARK_LEVEL;
  const bool alerte = mouvement && obscurite;
  digitalWrite(BUZZER_PIN, alerte ? HIGH : LOW);

  maintenirReseau();
  if (millis() - dernierePublication >= 2000) {
    dernierePublication = millis();
    if (mqtt.connected()) {
      mqtt.publish("airsense/esp32/pir", mouvement ? "1" : "0");
      mqtt.publish("airsense/esp32/ldr", obscurite ? "DARK" : "LIGHT");
      mqtt.publish("airsense/esp32/buzzer", alerte ? "ON" : "OFF");
    }
    Serial.printf("PIR=%d | LDR=%s | BUZZER=%s | MQTT=%s\n",
                  mouvement, obscurite ? "DARK" : "LIGHT",
                  alerte ? "ON" : "OFF", mqtt.connected() ? "OK" : "NON");
  }
  delay(20);
}
