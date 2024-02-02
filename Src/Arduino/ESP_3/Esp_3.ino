#include <WiFi.h>
#include <PubSubClient.h>
#include <NewPing.h>

// Informations de connexion WiFi
const char* ssid = "votre-ssid";
const char* password = "votre-mot-de-passe";

// Adresse IP et port du broker MQTT
const char* mqttBroker = "votre-adresse-ip-broker-mqtt";
const int mqttPort = 1883;

// Broches des capteurs à ultrasons
#define TRIGGER_PIN_1 12
#define ECHO_PIN_1 14
#define TRIGGER_PIN_2 25
#define ECHO_PIN_2 26
#define TRIGGER_PIN_3 27
#define ECHO_PIN_3 33

// Distance maximale et minimale pour les capteurs
#define MAX_DISTANCE 200 // 200 cm
#define MIN_DISTANCE 5   // 5 cm

// Sujets MQTT pour chaque capteur
const char* mqttTopics[] = {"bac/1", "bac/2", "bac/3"};

WiFiClient espClient; // Client WiFi pour la connexion
PubSubClient client(espClient); // Client MQTT

// Capteurs à ultrasons
NewPing sensor1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sensor2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE);
NewPing sensor3(TRIGGER_PIN_3, ECHO_PIN_3, MAX_DISTANCE);

// Tableaux pour stocker les distances mesurées par les capteurs
unsigned int distances[] = {0, 0, 0};
unsigned int lastDistances[] = {0, 0, 0};

void setup() {
  Serial.begin(115200); // Initialisation de la communication série

  // Connexion au réseau WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau Wi-Fi...");
  }
  Serial.println("Connecté au réseau Wi-Fi");

  // Configuration du client MQTT
  client.setServer(mqttBroker, mqttPort);
}

void loop() {
  if (!client.connected()) {
    reconnect(); // Reconnexion au broker MQTT si la connexion est perdue
  }

  client.loop(); // Gestion de la communication MQTT

  unsigned long currentTime = millis();

  // Mesurer la distance chaque seconde
  if (currentTime % 1000 == 0) {
    distances[0] = sensor1.ping_cm();
    distances[1] = sensor2.ping_cm();
    distances[2] = sensor3.ping_cm();

    // Vérifier si la distance a changé pour chaque capteur
    for (int i = 0; i < 3; i++) {
      if (distances[i] != lastDistances[i]) { // Si la distance a changé
        publishSensorValue(i, distances[i]); // Publier la nouvelle valeur MQTT
        lastDistances[i] = distances[i]; // Mettre à jour la dernière distance mesurée
      }
    }
  }

  delay(100); // Délai pour éviter les surcharges du processeur
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    if (client.connect("ESP32Client")) { // Connexion au broker MQTT
      Serial.println("Connecté");
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayer dans 5 secondes");
      delay(5000); // Attendre 5 secondes avant de réessayer
    }
  }
}

void publishSensorValue(int sensorIndex, unsigned int distance) {
  if (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE) { // Vérifier si la distance est valide
    char message[10];
    itoa(sensorIndex + 1, message, 10); // Convertir l'index du capteur en chaîne de caractères
    client.publish(mqttTopics[sensorIndex], message); // Publier la nouvelle valeur sur le sujet MQTT correspondant
    Serial.print("Capteur ");
    Serial.print(sensorIndex + 1);
    Serial.print(" : ");
    Serial.print(distance);
    Serial.println(" cm");
  }
}
