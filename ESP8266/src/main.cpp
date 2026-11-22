
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <math.h>
#include <PubSubClient.h>

// ============================================
// 1. CONFIGURATION DU MATERIEL
// ============================================

#define DHT_PIN D2
#define DHT_TYPE DHT11

#define MQ_PIN D1
#define LED_VERTE D5
#define LED_ROUGE D6
#define SWITCH_PIN D7

DHT dht(DHT_PIN, DHT_TYPE);
ESP8266WebServer server(80);

// ============================================
// 2. CONFIGURATION WIFI
// ============================================

#include "config.h"
// Reseau Wi-Fi local commun aux deux cartes et au PC Ubuntu.
// Les identifiants sont definis dans include/config.h.
const char* STA_SSID = WIFI_SSID;
const char* STA_PASSWORD = WIFI_PASSWORD;
const char* MQTT_SERVER = BROKER_IP;
const uint16_t MQTT_PORT = 1883;

WiFiClient mqttWifiClient;
PubSubClient mqtt(mqttWifiClient);
unsigned long dernierEssaiWifi = 0;
unsigned long dernierEssaiMQTT = 0;
unsigned long dernierePublication = 0;
const unsigned long PERIODE_MQTT = 3000;
const unsigned long RETRY_WIFI = 10000;
const unsigned long RETRY_MQTT = 5000;


// Dernieres informations du noeud ESP32 (MQTT).
// Chaque champ a sa propre date pour eviter de montrer une valeur ancienne.
bool pir32=false;
String ldr32="";
String buzzer32="";
unsigned long pirAt=0,ldrAt=0,buzzerAt=0;
const unsigned long ESP32_TIMEOUT=10000;

void recevoirMQTT(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT RX] Topic : ");
Serial.print(topic);

Serial.print(" | Valeur : ");

for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
}

Serial.println();
  String value;
  for(unsigned int i=0;i<length && i<32;i++) value += (char)payload[i];
  const unsigned long now=millis();
  String t(topic);
  if(t=="airsense/esp32/pir" && (value=="0" || value=="1")){pir32=(value=="1");pirAt=now?now:1;}
  else if(t=="airsense/esp32/ldr" && (value=="DARK" || value=="LIGHT")){ldr32=value;ldrAt=now?now:1;}
  else if(t=="airsense/esp32/buzzer" && (value=="ON" || value=="OFF")){buzzer32=value;buzzerAt=now?now:1;}
}

void envoyerESP32(){
  const unsigned long now=millis();
  bool p=pirAt && now-pirAt<ESP32_TIMEOUT;
  bool l=ldrAt && now-ldrAt<ESP32_TIMEOUT;
  bool b=buzzerAt && now-buzzerAt<ESP32_TIMEOUT;
  bool online=p&&l&&b;
  unsigned long age=online?now-min(min(pirAt,ldrAt),buzzerAt):0;
  String json="{\"online\":";json+=(online?"true":"false");
  json+=",\"pir\":";json+=(p?(pir32?"true":"false"):"null");
  json+=",\"ldr\":";json+=(l?"\""+ldr32+"\"":"null");
  json+=",\"buzzer\":";json+=(b?"\""+buzzer32+"\"":"null");
  json+=",\"age_ms\":"+String(age)+"}";
  server.sendHeader("Cache-Control","no-store");
  server.send(200,"application/json",json);
}

// ============================================
// 3. SEUILS DE SURVEILLANCE
// ============================================

// Seuils de demonstration, a adapter
const float TEMP_WARNING = 30.0;
const float TEMP_CRITICAL = 35.0;

const float HUM_MIN = 30.0;
const float HUM_MAX = 70.0;

// ============================================
// 4. VARIABLES DU SYSTEME
// ============================================

float temperature = NAN;
float humidite = NAN;

bool systemeON = false;
bool alerteMQ = false;
bool dhtValide = false;

String etat = "OFF";

unsigned long dernierDHT = 0;
unsigned long dernierLog = 0;

const unsigned long PERIODE_DHT = 2500;
const unsigned long PERIODE_LOG = 5000;

// ============================================
// 5. INTERFACE WEB
// ============================================

#include "airsense_dashboard.h"

// ============================================
// 6. ROUTE PAGE WEB
// ============================================

void afficherPage() {
  server.send_P(200, "text/html; charset=utf-8", PAGE_HTML);
}

// ============================================
// 7. ROUTE API JSON
// ============================================

void envoyerDonnees() {

  String json;
  json.reserve(240);

  json = "{";

  json += "\"on\":";
  json += systemeON ? "true" : "false";

  json += ",\"temperature\":";
  json += (systemeON && dhtValide) ?
    String(temperature,1) : "null";

  json += ",\"humidite\":";
  json += (systemeON && dhtValide) ?
    String(humidite,1) : "null";

  json += ",\"dht_ok\":";
  json += (systemeON && dhtValide) ?
    "true" : "false";

  json += ",\"mq\":";
  json += (systemeON && alerteMQ) ?
    "true" : "false";

  json += ",\"etat\":\"";
  json += etat;
  json += "\"";

  json += ",\"uptime\":";
  json += String(millis()/1000);

  json += ",\"sample\":";
  json += String(dernierDHT);

  json += "}";

  server.sendHeader("Cache-Control", "no-store");
  server.send(200,"application/json",json);
}

// ============================================
// 8. LECTURE DES CAPTEURS
// ============================================

void lireCapteurs() {

  // MQ : seuil active si DO = LOW
  alerteMQ = (digitalRead(MQ_PIN) == LOW);

  // DHT11 : lecture toutes les 2,5 secondes
  if(millis() - dernierDHT >= PERIODE_DHT) {

    dernierDHT = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    Serial.print("DEBUG DHT | T = ");
Serial.print(t);

Serial.print(" | H = ");
Serial.println(h);

    if(isnan(t) || isnan(h)) {
      dhtValide = false;
      temperature = NAN;
      humidite = NAN;
      Serial.println("Erreur lecture DHT11");
    }
    else {
      temperature = t;
      humidite = h;
      dhtValide = true;
    }
  }
}

// ============================================
// 9. CLASSIFICATION DES ETATS
// ============================================

void calculerEtat() {

  if(!systemeON) {
    etat = "OFF";
    return;
  }

  if(alerteMQ) {
    etat = "CRITICAL";
    return;
  }

  if(!dhtValide) {
    etat = "SENSOR_ERROR";
    return;
  }

  if(temperature >= TEMP_CRITICAL) {
    etat = "CRITICAL";
  }
  else if(temperature >= TEMP_WARNING ||
          humidite < HUM_MIN ||
          humidite > HUM_MAX) {
    etat = "WARNING";
  }
  else {
    etat = "NORMAL";
  }
}

// ============================================
// 10. COMMANDE DES LED
// ============================================


void gererLED() {

  if (!systemeON) {
    // Systeme desactive
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ROUGE, LOW);
  }
  else if (etat == "NORMAL") {
    // Conditions normales
    digitalWrite(LED_VERTE, HIGH);
    digitalWrite(LED_ROUGE, LOW);
  }
  else {
    // WARNING, CRITICAL ou SENSOR_ERROR
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ROUGE, HIGH);
  }
}
// ============================================
// 11. COMMUNICATION WIFI / MQTT (NON BLOQUANTE)
// ============================================

void maintenirConnexion() {
  unsigned long maintenant = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (dernierEssaiWifi == 0 || maintenant - dernierEssaiWifi >= RETRY_WIFI) {
      dernierEssaiWifi = maintenant;
      Serial.println("[WiFi STA] Tentative de connexion...");
      WiFi.begin(STA_SSID, STA_PASSWORD);
    }
    return;
  }

  if (mqtt.connected()) {
    mqtt.loop();
    return;
  }

  if (dernierEssaiMQTT == 0 || maintenant - dernierEssaiMQTT >= RETRY_MQTT) {
    dernierEssaiMQTT = maintenant;
    String clientId = "AirSense-ESP8266-" + String(ESP.getChipId(), HEX);
    if (mqtt.connect(clientId.c_str(),
                     "airsense/esp8266/availability", 1, true, "offline")) {
      Serial.println("[MQTT] Connecte au broker Ubuntu");
      mqtt.publish("airsense/esp8266/availability", "online", true);
     bool subPir = mqtt.subscribe("airsense/esp32/pir");
bool subLdr = mqtt.subscribe("airsense/esp32/ldr");
bool subBuzzer = mqtt.subscribe("airsense/esp32/buzzer");

Serial.printf(
    "[MQTT SUB] PIR=%d | LDR=%d | BUZZER=%d\n",
    subPir,
    subLdr,
    subBuzzer
);
      dernierePublication = maintenant - PERIODE_MQTT;
    } else {
      Serial.print("[MQTT] Connexion echouee, code = ");
      Serial.println(mqtt.state());
    }
  }
}

void publierMQTT() {
  if (!mqtt.connected()) return;
  if (millis() - dernierePublication < PERIODE_MQTT) return;
  dernierePublication = millis();

  mqtt.publish("airsense/esp8266/system", systemeON ? "ON" : "OFF", true);
  mqtt.publish("airsense/esp8266/status", etat.c_str(), true);
  mqtt.publish("airsense/esp8266/gas",
               !systemeON ? "INACTIVE" : (alerteMQ ? "ALERT" : "NORMAL"), true);

  // Ne jamais publier d'anciennes mesures lorsque le systeme est OFF
  // ou lorsqu'une lecture DHT11 a echoue.
  if (systemeON && dhtValide) {
    String t = String(temperature, 1);
    String h = String(humidite, 1);
    mqtt.publish("airsense/esp8266/temperature", t.c_str(), true);
    mqtt.publish("airsense/esp8266/humidity", h.c_str(), true);
  } else {
    mqtt.publish("airsense/esp8266/temperature", "", true);
    mqtt.publish("airsense/esp8266/humidity", "", true);
  }
}

// ============================================
// 11. INITIALISATION
// ============================================

void setup() {

  Serial.begin(115200);
  delay(200);

  pinMode(MQ_PIN,INPUT);
  pinMode(LED_VERTE,OUTPUT);
  pinMode(LED_ROUGE,OUTPUT);
  pinMode(SWITCH_PIN,INPUT_PULLUP);

  digitalWrite(LED_VERTE,LOW);
  digitalWrite(LED_ROUGE,LOW);

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  Serial.println();
  Serial.println("========================");
  Serial.println("       AirSense         ");
  Serial.println("========================");

  Serial.println("Mode Wi-Fi local (STA) uniquement");

  server.on("/",HTTP_GET,afficherPage);
  server.on("/api/data",HTTP_GET,envoyerDonnees);
  server.on("/api/esp32",HTTP_GET,envoyerESP32);

  server.begin();
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(recevoirMQTT);
  mqtt.setSocketTimeout(3);
  mqtt.setKeepAlive(20);
  WiFi.begin(STA_SSID, STA_PASSWORD);
  dernierEssaiWifi = millis();

  Serial.println("Serveur Web demarre");
  Serial.println("[MQTT] Connexion au Wi-Fi commun en cours");
Serial.println("===== CONFIGURATION RESEAU =====");

Serial.print("WiFi STA : ");
Serial.println(STA_SSID);

Serial.print("Etat WiFi local : ");
Serial.println(WiFi.status());

Serial.print("IP locale ESP8266 : ");
Serial.println(WiFi.localIP());
}

// ============================================
// 12. BOUCLE PRINCIPALE
// ============================================

void loop() {

  server.handleClient();
  maintenirConnexion();

  bool nouvelEtatON = (digitalRead(SWITCH_PIN) == LOW);

  if(nouvelEtatON != systemeON) {

    systemeON = nouvelEtatON;

    if(systemeON) {
      // Forcer une acquisition rapide
      dernierDHT = millis() - PERIODE_DHT;
      dhtValide = false;
      Serial.println("SYSTEME ON");
    }
    else {
      dhtValide = false;
      alerteMQ = false;
      Serial.println("SYSTEME OFF");
    }
  }

  if(systemeON) {
    lireCapteurs();
  }

  calculerEtat();
  gererLED();
  publierMQTT();

  if(millis() - dernierLog >= PERIODE_LOG) {

    dernierLog = millis();

    Serial.print("Etat : ");
    Serial.print(etat);

    Serial.print(" | T : ");
    if(dhtValide && systemeON)
      Serial.print(temperature);
    else
      Serial.print("--");

    Serial.print(" | H : ");
    if(dhtValide && systemeON)
      Serial.print(humidite);
    else
      Serial.print("--");

    Serial.print(" | MQ : ");
    Serial.println(
      systemeON && alerteMQ ?
      "ALERTE" : "NORMAL/INACTIF"
    );
  }

  delay(10);

static unsigned long dernierDiagnostic = 0;

if (millis() - dernierDiagnostic >= 5000) {
    dernierDiagnostic = millis();

    Serial.println("===== DIAGNOSTIC RESEAU =====");

    Serial.print("WiFi STA connecte : ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "OUI" : "NON");

    Serial.print("IP locale ESP8266 : ");
    Serial.println(WiFi.localIP());

    Serial.print("WiFi STA : ");
    Serial.println(STA_SSID);

    Serial.print("MQTT connecte : ");
    Serial.println(mqtt.connected() ? "OUI" : "NON");

    Serial.print("Memoire libre : ");
    Serial.println(ESP.getFreeHeap());

    Serial.println("============================");
}

}
