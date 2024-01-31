#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

const char *ssid = "votre-ssid";
const char *password = "votre-mot-de-passe";
const char *mqttBroker = "votre-adresse-ip-broker-mqtt";
const int mqttPort = 1883;

const int servoPin1 = 18;
const int servoPin2 = 19;

Servo servo1;
Servo servo2;

WiFiClient espClient;
PubSubClient client(espClient);

bool movementDone = true;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void servoTask(void *pvParameters) {
  const int delayTime = 10;
  const int step = 2;

  for (;;) {
    if (!movementDone) {
      portENTER_CRITICAL(&mux);
      for (int angle = 0; angle <= 90; angle += step) {
        servo1.write(angle);
        servo2.write(180 - angle);
        delay(delayTime);
      }
      delay(1000); // Attendre un court instant après le mouvement
      for (int angle = 90; angle >= 0; angle -= step) { // Revenir à la position initiale
        servo1.write(angle);
        servo2.write(180 - angle);
        delay(delayTime);
      }
      movementDone = true;
      portEXIT_CRITICAL(&mux);
    }
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau Wi-Fi...");
  }
  Serial.println("Connecté au réseau Wi-Fi");

  servo1.attach(servoPin1, 500, 2400);
  servo2.attach(servoPin2, 500, 2400);

  xTaskCreate(
    servoTask,
    "ServoTask",
    10000,
    NULL,
    1,
    NULL
  );

  client.setServer(mqttBroker, mqttPort);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  delay(500);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connecté");
      client.subscribe("servo/move");
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayer dans 5 secondes");
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message reçu [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    if (receivedChar == '1') {
      portENTER_CRITICAL(&mux);
      movementDone = false;
      portEXIT_CRITICAL(&mux);
    }
  }
  Serial.println();
}
