#include <WiFi.h> // Inclure la bibliothèque WiFi pour la connexion Wi-Fi
#include <PubSubClient.h> // Inclure la bibliothèque PubSubClient pour la communication MQTT
#include <ESP32Servo.h> // Inclure la bibliothèque ESP32Servo pour contrôler les servomoteurs

// Remplacer "your-ssid", "your-password" et "your-mqtt-broker-ip" par les valeurs appropriées
const char *ssid = "votre-ssid"; // SSID de votre réseau Wi-Fi
const char *password = "votre-mot-de-passe"; // Mot de passe de votre réseau Wi-Fi
const char *mqttBroker = "votre-adresse-ip-broker-mqtt"; // Adresse IP de votre broker MQTT
const int mqttPort = 1883; // Port MQTT par défaut

const int servoPin1 = 18; // Broche utilisée pour le premier servomoteur
const int servoPin2 = 19; // Broche utilisée pour le deuxième servomoteur

Servo servo1; // Déclarer le premier objet Servo
Servo servo2; // Déclarer le deuxième objet Servo

WiFiClient espClient; // Créer un objet de type WiFiClient pour la connexion Wi-Fi
PubSubClient client(espClient); // Créer un objet de type PubSubClient pour la communication MQTT

int positionChoice = 0; // Variable pour stocker le choix de position des servomoteurs

// Fonction pour contrôler les servomoteurs
void servoTask(void *pvParameters) {
  const int delayTime = 10; // Temps d'attente entre chaque étape de mouvement (en millisecondes)
  const int step = 2; // Pas de mouvement à chaque itération

  for (;;) {
    if (positionChoice == 0) { // Si le choix de position est 0
      // Faire passer le premier servomoteur de 0 à 90 degrés
      for (int angle = 0; angle <= 90; angle += step) {
        servo1.write(angle); // Déplacer le premier servomoteur à l'angle actuel
        servo2.write(180 - angle); // Déplacer le deuxième servomoteur à l'angle symétrique
        delay(delayTime); // Attendre un court instant
      }
    } else if (positionChoice == 1) { // Sinon, si le choix de position est 1
      // Faire passer les deux servomoteurs de 180 à 90 degrés symétriquement
      for (int angle = 180; angle >= 90; angle -= step) {
        servo1.write(angle); // Déplacer le premier servomoteur à l'angle actuel
        servo2.write(180 - angle); // Déplacer le deuxième servomoteur à l'angle symétrique
        delay(delayTime); // Attendre un court instant
      }
    }

    delay(1000); // Attendre 1 seconde avant de recommencer
  }
}


void setup() {
  Serial.begin(115200); // Initialiser la communication série à 115200 bits par seconde

  WiFi.begin(ssid, password); // Se connecter au réseau Wi-Fi
  while (WiFi.status() != WL_CONNECTED) { // Attendre jusqu'à ce que la connexion Wi-Fi soit établie
    delay(1000);
    Serial.println("Connexion au réseau Wi-Fi...");
  }
  Serial.println("Connecté au réseau Wi-Fi");

  servo1.attach(servoPin1, 500, 2400); // Attacher le premier servomoteur à la broche spécifiée
  servo2.attach(servoPin2, 500, 2400); // Attacher le deuxième servomoteur à la broche spécifiée

  xTaskCreate(
    servoTask, // Nom de la fonction de la tâche
    "ServoTask", // Nom de la tâche
    10000, // Taille de la pile de la tâche (en mots)
    NULL, // Paramètres de la tâche
    1, // Priorité de la tâche
    NULL // Identificateur de la tâche
  );

  client.setServer(mqttBroker, mqttPort); // Configurer le broker MQTT avec l'adresse IP et le port
}

void loop() {
  if (!client.connected()) { // Si le client MQTT n'est pas connecté
    reconnect(); // Tenter de se reconnecter
  }

  client.loop(); // Maintenir la connexion MQTT

  delay(500); // Attendre 0.5 seconde
}

void reconnect() {
  while (!client.connected()) { // Tant que le client MQTT n'est pas connecté
    Serial.print("Tentative de connexion MQTT...");
    if (client.connect("ESP32Client")) { // Se connecter au broker MQTT avec un nom client
      Serial.println("Connecté");
      client.subscribe("servo/positionChoice"); // S'abonner au sujet pour recevoir les commandes de position
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayer dans 5 secondes");
      delay(5000); // Attendre 5 secondes avant de réessayer
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
    if (isdigit(receivedChar)) { // Si le caractère reçu est un chiffre
      positionChoice = receivedChar - '0'; // Convertir le caractère en nombre
    }
  }
  Serial.println();
}
