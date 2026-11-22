# AirSense — IoT Laboratory Monitoring

Prototype de surveillance environnementale et de détection d'intrusion basé sur **ESP8266**, **ESP32**, **MQTT Mosquitto** et un **dashboard web local**.

> Statut : prototype testé en réseau local. Ne constitue pas un système certifié de détection de gaz, d'incendie ou de sécurité.

## Fonctionnalités

- **ESP8266 (NodeMCU)** : lecture DHT11 (température et humidité), entrée numérique MQ (seuil gaz), interrupteur ON/OFF, LED verte/rouge, serveur web et API JSON.
- **ESP32** : PIR, LDR numérique et buzzer. Alarme locale lorsque mouvement **ET** obscurité.
- **MQTT** : publications des deux nœuds via Mosquitto sur un PC du réseau local ; l'ESP8266 s'abonne aux états ESP32.
- **Dashboard** : mesures, graphique historique court, seuils de démonstration, événements récents et état du nœud ESP32.

## Architecture

```text
ESP32 (PIR / LDR / buzzer) ---- MQTT ----> Mosquitto (Ubuntu, port 1883)
                                            ^             |
                                            |             v
ESP8266 (DHT11 / MQ / LEDs / switch) -- MQTT publish/subscribe
             |
             +---- HTTP local ----> Navigateur : dashboard AirSense
```

Les deux cartes et le PC broker doivent être sur un réseau Wi-Fi local compatible (2,4 GHz pour les cartes). Aucune exposition à Internet n'est nécessaire.

## Matériel et connexions

| Carte | Élément | Broche |
|---|---|---|
| ESP8266 | DHT11 | D2 / GPIO4 |
| ESP8266 | MQ, sortie numérique DO | D1 / GPIO5 |
| ESP8266 | LED verte | D5 |
| ESP8266 | LED rouge | D6 |
| ESP8266 | Interrupteur | D7 |
| ESP32 | PIR, sortie numérique | GPIO27 |
| ESP32 | LDR, sortie numérique | GPIO26 |
| ESP32 | Buzzer | GPIO25 |

Vérifier l'alimentation, la masse commune, les résistances des LED et la compatibilité **3,3 V** des signaux avant le branchement. Les broches et le comportement peuvent dépendre des modules utilisés.

## Installation

Prérequis : VS Code + PlatformIO, Mosquitto sur Ubuntu, Wi-Fi local.

1. Copier `ESP8266/include/config.example.h` vers `ESP8266/include/config.h` et faire de même dans `ESP32/include/`. Renseigner le SSID, le mot de passe et l'IP locale du PC broker dans **les deux** fichiers `config.h`.
2. Vérifier que Mosquitto écoute sur le réseau local (`sudo ss -lntp | grep 1883`). Ne pas ouvrir le port 1883 sur Internet. Le prototype utilise MQTT sans TLS/authentification : utiliser uniquement un réseau de confiance.
3. Compiler et téléverser depuis chaque sous-projet :

```bash
cd ESP8266
pio run -e nodemcuv2
pio device list
pio run -e nodemcuv2 -t upload --upload-port /dev/ttyUSBX

cd ../ESP32
pio run -e esp32dev
pio device list
pio run -e esp32dev -t upload --upload-port /dev/ttyUSBX
```

Remplacer `/dev/ttyUSBX` par le port identifié pour **chaque carte**. Il peut changer après reconnexion USB.

4. Sur Ubuntu, vérifier la réception :

```bash
mosquitto_sub -h IP_DU_PC_BROKER -t 'airsense/#' -v
```

5. Lire l'IP locale ESP8266 dans son moniteur série, puis ouvrir `http://IP_ESP8266/` dans un navigateur du même réseau.

## API HTTP de l'ESP8266

- `GET /` : dashboard.
- `GET /api/data` : état de l'ESP8266, température, humidité et gaz.
- `GET /api/esp32` : dernier état PIR, LDR, buzzer reçu via MQTT et fraîcheur des messages.

## Topics MQTT

| Topic | Exemple |
|---|---|
| `airsense/esp32/pir` | `0` / `1` |
| `airsense/esp32/ldr` | `LIGHT` / `DARK` |
| `airsense/esp32/buzzer` | `OFF` / `ON` |
| `airsense/esp8266/temperature` | `24.5` |
| `airsense/esp8266/humidity` | `49.0` |
| `airsense/esp8266/gas` | `NORMAL` / `ALERT` / `INACTIVE` |
| `airsense/esp8266/status` | `NORMAL` / `WARNING` / `CRITICAL` / `OFF` / `SENSOR_ERROR` |
| `airsense/esp8266/system` | `ON` / `OFF` |
| `airsense/esp8266/availability` | `online` / `offline` |

## Validation

Vérifié sur le prototype : publication périodique PIR/LDR/buzzer par ESP32, affichage des valeurs DHT11/MQ et réception MQTT ESP32 dans le dashboard ESP8266 (capture du 22/11/2026). Les scénarios d'alarme et la robustesse aux coupures réseau nécessitent encore des essais complets.

## Limites et sécurité

- Le module MQ avec sortie numérique indique un **franchissement de seuil**, pas une concentration en ppm calibrée.
- Les seuils de température/humidité sont des **valeurs de démonstration**.
- La mesure de lumière `LIGHT`/`DARK` dépend du réglage du comparateur du module LDR.
- Ne pas publier de secrets dans Git. `config.h` est ignoré par `.gitignore` ; `config.example.h` ne contient que des exemples.
- Les adresses IP peuvent changer : envisager une réservation DHCP pour le broker et l'ESP8266.

## Structure

```text
ESP8266/src/main.cpp                 # logique capteurs + MQTT + serveur web
ESP8266/include/airsense_dashboard.h # interface web embarquée
ESP8266/include/config.example.h     # modèle de configuration
ESP32/src/main.cpp                   # PIR/LDR/buzzer + MQTT
ESP32/include/config.example.h       # modèle de configuration
```

## Licence

Aucune licence open source n'est accordée pour le moment. Une licence pourra être ajoutée après la vérification du dépôt et avant son éventuelle publication publique.
