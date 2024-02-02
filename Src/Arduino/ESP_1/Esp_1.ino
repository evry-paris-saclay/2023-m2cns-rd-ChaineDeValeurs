#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Informations de connexion WiFi et MQTT
const char* ssid = "votre-ssid"; // SSID de votre réseau Wi-Fi
const char* password = "votre-mot-de-passe"; // Mot de passe de votre réseau Wi-Fi
const char* mqttBroker = "votre-adresse-ip-broker-mqtt"; // Adresse IP de votre broker MQTT
const int mqttPort = 1883; // Port MQTT par défaut

// Broches utilisées pour les servomoteurs
const int servoPin1 = 18;
const int servoPin2 = 19;
const int servoPin3 = 21;
const int servoPin4 = 22;

// Création des objets Servo pour contrôler les servomoteurs
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// Variable pour indiquer la position actuelle
int currentPosition = 0;

// Déclaration des positions pour les servomoteurs
const int positions[3][4] = {
  {0, 180, 90, 90},   // Position 0
  {60, 120, 60, 120}, // Position 1
  {90, 90, 0, 180}    // Position 2
};

WiFiClient espClient;
PubSubClient client(espClient);

// Fonction de configuration initiale
void setup() {
  Serial.begin(115200); // Initialisation de la communication série

  // Connexion au réseau Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau Wi-Fi...");
  }
  Serial.println("Connecté au réseau Wi-Fi");

  // Attachement des servomoteurs aux broches spécifiées
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);

  // Configuration du client MQTT
  client.setServer(mqttBroker, mqttPort);
  client.setCallback(callback);

  // Reconnexion au broker MQTT
  if (!client.connected()) {
    reconnect();
  }
}

// Fonction de boucle principale
void loop() {
  client.loop(); // Maintien de la connexion MQTT
}

// Fonction de reconnexion au broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connecté");
      client.subscribe("servo/position"); // Abonnement au sujet pour recevoir les commandes de position
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayer dans 5 secondes");
      delay(5000);
    }
  }
}

// Fonction de rappel (callback) appelée lorsqu'un message MQTT est reçu
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Récupération de la nouvelle position depuis le message MQTT
  int newPosition = payload[0] - '0';
  
  // Vérification de la validité de la nouvelle position
  if (newPosition >= 0 && newPosition <= 2) {
    moveToPosition(newPosition); // Déplacement vers la nouvelle position
  }
}

// Fonction pour déplacer les servomoteurs vers la nouvelle position demandée
void moveToPosition(int newPosition) {
  if (newPosition != currentPosition) { // Vérification si la nouvelle position est différente de la position actuelle
    for (int i = 0; i < 2; i++) { // Boucle sur deux paires de servomoteurs
      int currentAngle1 = positions[currentPosition][i];
      int currentAngle2 = positions[currentPosition][i+2];
      int targetAngle1 = positions[newPosition][i];
      int targetAngle2 = positions[newPosition][i+2];
      int step1 = (targetAngle1 > currentAngle1) ? 1 : -1;
      int step2 = (targetAngle2 > currentAngle2) ? 1 : -1;

      // Déplacement progressif des paires de servomoteurs vers la nouvelle position
      for (int angle1 = currentAngle1, angle2 = currentAngle2; angle1 != targetAngle1 || angle2 != targetAngle2; angle1 += step1, angle2 += step2) {
        servo1.write(angle1);
        servo2.write(angle2);
        servo3.write(angle1); // Les servomoteurs 3 et 4 suivent les servomoteurs 1 et 2
        servo4.write(angle2);
        delay(10); // Délai pour un mouvement progressif
      }
    }
    currentPosition = newPosition; // Mise à jour de la position actuelle
    client.publish("servo/position", String(currentPosition).c_str()); // Envoi d'un message de confirmation de la position atteinte
  }
}
