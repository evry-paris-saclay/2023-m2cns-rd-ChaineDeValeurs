#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

const char *ssid = "your-ssid";
const char *password = "your-password";
const char *mqttBroker = "your-mqtt-broker-ip";
const int mqttPort = 1883;

const int servoPin1 = 18;
const int servoPin2 = 19;

Servo servo1;
Servo servo2;

WiFiClient espClient;
PubSubClient client(espClient);

int positionChoice = 0;

void servoTask(void *pvParameters) {
  for (;;) {
    if (positionChoice == 0) {
      servo1.write(0);
      servo2.write(0);
    } else if (positionChoice == 1) {
      servo1.write(90);
      servo2.write(90);
    }

    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

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
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("servo/positionChoice");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    if (isdigit(receivedChar)) {
      positionChoice = receivedChar - '0';
    }
  }
  Serial.println();
}
