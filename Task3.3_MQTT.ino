#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <Arduino.h>

#define LED_LIST_MAX_SIZE 15

// Please enter your sensitive data in the Secret tab
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)
String apikey = SECRET_APIKEY;
WiFiClient wifiClient;
WiFiClient client;
MqttClient mqttClient(wifiClient);

String eventName = "MessageReceived";
char HOST_NAME[] = "maker.ifttt.com";
// String PATH_NAME = "/trigger/" + eventName + "/with/key/" + apikey; // change your EVENT-NAME and YOUR-KEY
String PATH_NAME = "https://maker.ifttt.com/trigger/MessageReceived/with/key/cx7CHJK6C7zmi0uktrjDS3";
const char broker[] = "broker.emqx.io";
int port = 1883;

const String topicWave = "/SIT730/Wave";
const String topicPat = "/SIT730/Pat";
String messageMQTT = "PhilipWilliams";

const int LEDpin = 4;
const int trigPin = 2;
const int echoPin = 3;
const int sampleSize = 5;
int distance_samples[sampleSize];
int sample_index = 0;
int waveCount = 0;

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
int LEDlistSize = 0;       // The current size of the list

void setup()
{
  setupSerial();
  setupWiFi();
  setupMQTT();
  topicSubscriptionMQTT();
  setupHC_SR04();
  pinMode(LEDpin, OUTPUT);
}

void setupSerial()
{
  Serial.begin(9600);
  while (!Serial)
    ;
}

void setupWiFi()
{
  // initialize WiFi connection
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();
}

void setupMQTT()
{
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1)
      ;
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  mqttClient.onMessage(onMqttMessage);
}

void topicSubscriptionMQTT()
{
  mqttClient.subscribe(topicWave);
  mqttClient.subscribe(topicPat);

  Serial.println("Subscribing to topics: ");
  Serial.print("Topic: ");
  Serial.println(topicWave);
  Serial.print("Topic: ");
  Serial.println(topicPat);
}

void setupHC_SR04()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop()
{
  mqttClient.poll();

  int curr_distance = measure_distance();
  update_samples(curr_distance);
  delay(70);

  if (detect_moving_towards())
  {
    Serial.println("Hand Pat detected!");
    MQTTSend(topicPat);
    clear_samples();
  }
  else if (detect_wave())
  {
    Serial.println("Hand wave detected!");
    MQTTSend(topicWave);
    clear_samples();
  }
  waveCheck();
  LedBlink();
}

float measure_distance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int distance = (duration / 2) * 0.0344;
  return distance;
}

void update_samples(int new_sample)
{
  distance_samples[sample_index] = new_sample;
  sample_index = (sample_index + 1) % sampleSize;
}

void clear_samples()
{
  for (int i = 0; i < sampleSize; i++)
  {
    distance_samples[i] = 0;
  }
}

bool detect_wave()
{
  int count = 0;

  bool matchPart1 = false;
  bool matchPart2 = false;
  bool matchPart3 = false;

  for (int i = sample_index; count < sampleSize; i = (i + 1) % sampleSize)
  {
    count++;
    if (distance_samples[i] > 100 && matchPart2 == false && matchPart3 == false)
    {
      matchPart1 = true;
    }
    else if (distance_samples[i] < 100 && matchPart1 == true && matchPart3 == false)
    {
      matchPart2 = true;
    }
    else if (distance_samples[i] > 100 && matchPart1 == true && matchPart2 == true)
    {
      matchPart3 = true;
      // debugPatterns("Wave", true);
      return true;
    }
  }
  return false;
}

bool detect_moving_towards()
{
  int count = 0;
  int trigger = 0;
  for (int i = sample_index; count < sampleSize; i = (i + 1) % sampleSize)
  {
    if (trigger == 2)
    {
      // debugPatterns("Pat", true);
      return true;
    }
    count++;
    if (distance_samples[i] < distance_samples[(i + 1) % sampleSize] && distance_samples[i] < 100 && distance_samples[i] != 0)
    {
      trigger++;
    }
    else
    {
      // debugPatterns("Pat", false);
      return false;
    }
  }
  return false;
}

void MQTTSend(String topic)
{
  mqttClient.beginMessage(topic);
  mqttClient.print(messageMQTT);
  mqttClient.endMessage();
}

void onMqttMessage(int messageSize)
{
  String pattern = "";
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  if (mqttClient.messageTopic() == topicWave)
  {
    addToLEDList("pattern1");
    waveCount++;
    Serial.print("Wave Count: ");
    Serial.println(waveCount);
  }
  else if (mqttClient.messageTopic() == topicPat)
  {
    addToLEDList("pattern2");
  }

  // use the Stream interface to print the contents
  while (mqttClient.available())
  {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();
  Serial.println();
}


void LedBlink()
{

  if (currentPattern == "None")
  {
    currentPattern = processLEDList();
  }

  if (currentPattern == "pattern1")
  {
    if (millis() - pattern1lastTime1 > pattern1Interval)
    {
      digitalWrite(LEDpin, !digitalRead(LEDpin));
      pattern1BlinkCount++;
      if (pattern1BlinkCount >= pattern1BlinkQty)
      {
        currentPattern = "None";
        pattern1BlinkCount = 0;
      }
      pattern1lastTime1 = millis();
    }
  }
  else if (currentPattern == "pattern2")
  {
    if (millis() - pattern2lastTime2 > pattern2Interval)
    {
      digitalWrite(LEDpin, !digitalRead(LEDpin));
      pattern2BlinkCount++;
      if (pattern2BlinkCount >= pattern2BlinkQty)
      {
        currentPattern = "None";
        pattern2BlinkCount = 0;
      }
      pattern2lastTime2 = millis();
    }
  }
}

void waveCheck()
{
  if (waveCount == 10)
  {
    String subject = "10%20Waves%20Detected";
    String body = "Someone%20waved%20at%20the%20sensor%2010%20times";
    serverSend(subject, body);
    waveCount = 0;
  }
}

void serverSend(String subject, String body)
{
  String queryString = "?value1=" + subject + "&value2=" + body;

  // connect to web server on port 80:
  if (client.connect(HOST_NAME, 80))
  {
    // if connected:
    Serial.println("Connected to server");
  }
  else
  { // if not connected:
    Serial.println("connection failed");
  }

  // make a HTTP request:
  // send HTTP header
  client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
  client.println("Host: " + String(HOST_NAME));
  client.println("Connection: close");
  client.println(); // end HTTP header

  while (client.connected())
  {
    if (client.available())
    {
      // read an incoming byte from the server and print it to serial monitor:
      char c = client.read();
      Serial.print(c);
    }
  }

  // the server's disconnected, stop the client:
  client.stop();
  Serial.println();
  Serial.println("disconnected");
}

void debugPatterns(String Trigger, bool matched)
{
  int debug_count = 0;
  Serial.println("******************************************************************************************************");
  Serial.print(Trigger);
  Serial.print(": ");
  if (matched)
  {
    Serial.println("Matched");
  }
  else
  {
    Serial.println("No Match");
  }
  Serial.print("sample index: ");
  Serial.println(sample_index);
  Serial.print("Test sequence: ");
  for (int i = sample_index; debug_count < sampleSize; i = (i + 1) % sampleSize)
  {
    Serial.print("Index: ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(distance_samples[i]);
    Serial.print(" ");
    debug_count++;
  }
  Serial.println();
  Serial.println("******************************************************************************************************");
}

void addToLEDList(String pattern)
{
  if (LEDlistSize < LED_LIST_MAX_SIZE)
  {
    LED_LIST[LEDlistSize] = pattern;
    LEDlistSize++;
  }
  else
  {
    Serial.println("Unable to process more than 15 LED patterns");
  }
}

String processLEDList()
{
  if (LEDlistSize > 0)
  {
    String pattern = LED_LIST[0];
    
    for (int i = 1; i < LEDlistSize; i++)
    {
      LED_LIST[i - 1] = LED_LIST[i];
    }

    LEDlistSize--;
    return pattern;
  }
  else
  {
    return "None";
  }
}
