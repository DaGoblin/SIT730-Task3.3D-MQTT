#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <Arduino.h>

#define LED_LIST_MAX_SIZE 15

//Wifi Connection Details
// Please enter your sensitive data in the Secret tab
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)
String apikey = SECRET_APIKEY;
WiFiClient wifiClient;
WiFiClient client;

// IFTTT Connection Details
String eventName = "MessageReceived";
char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME = "/trigger/" + eventName + "/with/key/" + apikey; // change your EVENT-NAME and YOUR-KEY
const char broker[] = "broker.emqx.io";
int port = 1883;

//MQTT Connection Details
MqttClient mqttClient(wifiClient);
const String topicWave = "/SIT730/Wave";
const String topicPat = "/SIT730/Pat";
String messageMQTT = "PhilipWilliams";

//HC-SR04 Sensor and LED Details
const int LEDpin = 4;
const int trigPin = 2;
const int echoPin = 3;
const int sampleSize = 5;
int distance_samples[sampleSize];
int sample_index = 0;
int waveCount = 0;

//LED Blink Pattern Details
// timers for led blink patterns
int pattern1Interval = 250;
int pattern2Interval = 150;
unsigned long pattern1lastTime1 = 0;
unsigned long pattern2lastTime2 = 0;

// For number of times we want the LED to blink multiple by 2 (for on and off)
int pattern1BlinkQty = 3 * 2;
int pattern2BlinkQty = 5 * 2;
int pattern1BlinkCount = 0;
int pattern2BlinkCount = 0;
String currentPattern = "None";

String LED_LIST[LED_LIST_MAX_SIZE]; // The array to hold the strings
int LEDlistSize = 0;                // The current size of the list

void setup()
{
  setupSerial();
  setupWiFi();
  setupMQTT();
  topicSubscriptionMQTT();
  setupHC_SR04();
  setupLED();
}

void loop()
{
  mqttClient.poll();
  sensorUpdate();
  handPatternCheck();
  waveCheck();
  LedBlink();
}
