#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Définition des informations de connexion WiFi et MQTT
const char *ssid = "votre-ssid"; // SSID de votre réseau Wi-Fi
const char *password = "votre-mot-de-passe"; // Mot de passe de votre réseau Wi-Fi
const char *mqttBroker = "votre-adresse-ip-broker-mqtt"; // Adresse IP de votre broker MQTT
const int mqttPort = 1883; // Port MQTT par défaut

// Définition des broches utilisées pour les servomoteurs
const int servoPin1 = 18; // Broche utilisée pour le premier servomoteur
const int servoPin2 = 19; // Broche utilisée pour le deuxième servomoteur

// Création des objets Servo pour contrôler les servomoteurs
Servo servo1;
Servo servo2;

// Création des objets pour la connexion Wi-Fi et MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Variable booléenne pour indiquer si le mouvement a été effectué
bool movementDone = true;

// Déclaration d'un verrou pour protéger l'accès à la variable partagée
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// Fonction pour contrôler les servomoteurs
void servoTask(void *pvParameters) {
  const int delayTime = 10; // Temps d'attente entre chaque étape de mouvement (en millisecondes)
  const int step = 2; // Pas de mouvement à chaque itération

  for (;;) {
    if (!movementDone) { // Si le mouvement n'a pas encore été effectué
      portENTER_CRITICAL(&mux); // Entrée dans une section critique (verrouillage)
      // Mouvement des servomoteurs de 0 à 90 degrés
      for (int angle = 0; angle <= 90; angle += step) {
        servo1.write(angle);
        servo2.write(180 - angle);
        delay(delayTime);
      }
      delay(1000); // Attendre un court instant après le mouvement
      // Retour des servomoteurs à leur position initiale (90 degrés)
      for (int angle = 90; angle >= 0; angle -= step) {
        servo1.write(angle);
        servo2.write(180 - angle);
        delay(delayTime);
      }
      movementDone = true; // Indiquer que le mouvement a été effectué
      portEXIT_CRITICAL(&mux); // Sortie de la section critique (déverrouillage)
    }
    delay(1000); // Attendre 1 seconde avant de vérifier à nouveau
  }
}

// Fonction de configuration de la connexion Wi-Fi et MQTT
void setup() {
  Serial.begin(115200); // Initialisation de la communication série à 115200 bits par seconde

  // Connexion au réseau Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau Wi-Fi...");
  }
  Serial.println("Connecté au réseau Wi-Fi");

  // Attachement des servomoteurs aux broches spécifiées
  servo1.attach(servoPin1, 500, 2400);
  servo2.attach(servoPin2, 500, 2400);

  // Création de la tâche pour contrôler les servomoteurs
  xTaskCreate(
    servoTask, // Fonction de la tâche
    "ServoTask", // Nom de la tâche
    10000, // Taille de la pile de la tâche (en mots)
    NULL, // Paramètres de la tâche
    1, // Priorité de la tâche
    NULL // Identificateur de la tâche
  );

  // Configuration du client MQTT avec l'adresse IP et le port du broker
  client.setServer(mqttBroker, mqttPort);
}

// Fonction de boucle principale
void loop() {
  if (!client.connected()) {
    reconnect(); // Reconnexion si le client MQTT n'est pas connecté
  }

  client.loop(); // Maintien de la connexion MQTT

  delay(500); // Attente de 0.5 seconde entre chaque itération
}

// Fonction de reconnexion au broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connecté");
      client.subscribe("servo/move"); // Abonnement au sujet pour recevoir les commandes de mouvement
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayer dans 5 secondes");
      delay(5000); // Attendre 5 secondes avant de réessayer la connexion
    }
  }
}

// Fonction de rappel (callback) appelée lorsqu'un message MQTT est reçu
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message reçu [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    if (receivedChar == '1') {
      portENTER_CRITICAL(&mux); // Entrée dans une section critique (verrouillage)
      movementDone = false; // Indiquer qu'un nouveau mouvement est nécessaire
      portEXIT_CRITICAL(&mux); // Sortie de la section critique (déverrouillage)
    }
  }
  Serial.println();
}
