#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "Nodemcu.h"

const PROGMEM char* ssid = "rs500m-d637f8-1";
const PROGMEM char* password = "affc0e85b9910";

const PROGMEM char* MQTT_CLIENT_ID = "tank-1goki";
const PROGMEM char* MQTT_SERVER_IP = "192.168.1.19";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "[Redacted]";
const PROGMEM char* MQTT_PASSWORD = "[Redacted]";

const PROGMEM char* MQTT_MOVE_TOPIC = "tank/1goki/move";

#define BPWM  D0
#define BIN1  D2
#define BIN2  D1
#define STBY  D4
#define APWM  D5
#define AIN1  D6
#define AIN2  D7

#define M_LEFT 1
#define M_RIGHT 2
#define M_FORWARD HIGH
#define M_BACKWARD LOW

WiFiClient espClient;
PubSubClient client(espClient);

int motorLeft = 0;
int motorRight = 0;

void setup() {
  Serial.begin(9600);
  setupWifiAndMQTT();

  pinMode(APWM, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BPWM, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);
}

/**
 * side = 1 -> left, 2 -> right
 * speed = 1 -> 10, <0 means backward
 */
void move(uint8_t side, int speed=6) {
  uint8_t p0 = (side == M_LEFT ? BPWM : APWM);
  uint8_t p1 = (side == M_LEFT ? BIN1 : AIN1);
  uint8_t p2 = (side == M_LEFT ? BIN2 : AIN2);
  analogWrite(p0, map(abs(speed), 0, 10, 0, 1023));

  int dir = speed > 0 ? M_FORWARD : M_BACKWARD;
  digitalWrite(p1, !dir);
  digitalWrite(p2, dir);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  move(M_LEFT, motorLeft);
  move(M_RIGHT, motorRight);
  delay(10);

  client.loop();
}

void setupWifiAndMQTT() {
  WiFi.begin(ssid, password);

  String msg = "Connecting to " + String(ssid) ;
  Serial.print(msg);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");

      // // Once connected, publish an announcement...
      // // publish the initial values
      // publishRGBState();
      // publishRGBBrightness();
      // publishRGBColor();

      // ... and resubscribe
      client.subscribe(MQTT_MOVE_TOPIC);
      // client.subscribe(MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC);
      // client.subscribe(MQTT_LIGHT_RGB_COMMAND_TOPIC);
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  // Serial.printf("%s payload:", p_topic);
  // Serial.println(payload);

  // handle message topic
  if (String(MQTT_MOVE_TOPIC).equals(p_topic)) {
    uint8_t firstIndex = payload.indexOf(',');
    String left = payload.substring(0, firstIndex);
    String right = payload.substring(firstIndex + 1);
    motorLeft = left.toInt();
    motorRight = right.toInt();

    // Serial.print(motorLeft);
    // Serial.print(",");
    // Serial.print(motorRight);
    // Serial.println("");

    // // test if the payload is equal to "ON" or "OFF"
    // if (payload.equals(String(LIGHT_ON))) {
    //   if (G_rgb_state != true) {
    //     G_rgb_state = true;
    //     publishRGBState();
    //   }
    // } else if (payload.equals(String(LIGHT_OFF))) {
    //   if (G_rgb_state != false) {
    //     G_rgb_state = false;
    //     publishRGBState();
    //   }
    // }
  // } else if (String(MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC).equals(p_topic)) {
  //   uint8_t brightness = payload.toInt();
  //   if (brightness < 0 || brightness > 100) {
  //     // do nothing...
  //     return;
  //   } else {
  //     G_rgb.brightness = brightness;
  //     publishRGBBrightness();
  //   }
  // } else if (String(MQTT_LIGHT_RGB_COMMAND_TOPIC).equals(p_topic)) {
  //   // get the position of the first and second commas
  //   uint8_t firstIndex = payload.indexOf(',');
  //   uint8_t lastIndex = payload.lastIndexOf(',');

  //   uint8_t rgb_red = payload.substring(0, firstIndex).toInt();
  //   if (rgb_red < 0 || rgb_red > 255) {
  //     return;
  //   } else {
  //     G_rgb.r = rgb_red;
  //   }

  //   uint8_t rgb_green = payload.substring(firstIndex + 1, lastIndex).toInt();
  //   if (rgb_green < 0 || rgb_green > 255) {
  //     return;
  //   } else {
  //     G_rgb.g = rgb_green;
  //   }

  //   uint8_t rgb_blue = payload.substring(lastIndex + 1).toInt();
  //   if (rgb_blue < 0 || rgb_blue > 255) {
  //     return;
  //   } else {
  //     G_rgb.b = rgb_blue;
  //   }

  //   publishRGBColor();
  }
}

