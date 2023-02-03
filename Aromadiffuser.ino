#include <ESP8266WiFi.h>
#include "ESP8266mDNS.h"
#include "WiFiUdp.h"
#include "ArduinoOTA.h"
#include <PubSubClient.h>

const char* ssid = "";
const char* password = "";

const char *mqtt_broker = "192.168.0.111";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

bool previousFanState = false;
long lastFanStateChangeTime = 0;
bool shouldHandleFanStart = false;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void printLog(const char *message) {
  Serial.println(message);
  //mqttClient.publish("aromadiffuser/debug", message);
}

void pressHardwareButton(bool useLongPressure) {
  digitalWrite(D7, HIGH);
  delay(useLongPressure ? 3500 : 1000);
  digitalWrite(D7, LOW);
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  printLog("New MQTT message");
  Serial.print("Topic: ");
  Serial.println(topic);
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message: ");
  printLog(message.c_str());
  if (strcmp(topic, "aromadiffuser/command") == 0) {
    if (message == "press") {
      pressHardwareButton(false);
    } else if (message == "press_long") {
      pressHardwareButton(true);
    }
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected with IP ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("Aromadiffuser");

  ArduinoOTA.onStart([]() {
    printLog("OTA started");
  });
  ArduinoOTA.onEnd([]() {
    printLog("OTA ended");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) printLog("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) printLog("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) printLog("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) printLog("Receive Failed");
    else if (error == OTA_END_ERROR) printLog("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA settled up");

  Serial.println("Setup pins");
  pinMode(D7, OUTPUT);
  pinMode(D6, INPUT);

  Serial.println("Running");
}

void connectMqtt() {
  Serial.println("Connecting MQTT");
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  while (!mqttClient.connected()) {
    if (mqttClient.connect("aromadiffuser", mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("MQTT connection failed with state ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
  mqttClient.subscribe("aromadiffuser/command");
}

void loop() {
  if (!mqttClient.connected())  {
    connectMqtt();
  }
  mqttClient.loop();

  bool isFanWorking = digitalRead(D6) == HIGH;

  if (previousFanState != isFanWorking) {
    if (isFanWorking) {
      printLog("Fan now enabled");
      shouldHandleFanStart = true;
    } else {
      printLog("Fan now disabled");
      shouldHandleFanStart = false;
      if (millis() - lastFanStateChangeTime < 1000) {
        printLog("Fan was working less than 1 second: no water error");
        mqttClient.publish("aromadiffuser/state", "error");
      } else {
        printLog("Fan was working for a long time: device was swtched off");
        mqttClient.publish("aromadiffuser/state", "off");
      }
    }
    previousFanState = isFanWorking;
    lastFanStateChangeTime = millis();
  }

  if (shouldHandleFanStart && millis() - lastFanStateChangeTime > 1000) {
    printLog("Fan is working more than 1 second: device successfully switched on");
    shouldHandleFanStart = false;
    mqttClient.publish("aromadiffuser/state", "on");
  }
}
