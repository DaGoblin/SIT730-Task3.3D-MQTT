
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

void setupLED()
{
    pinMode(LEDpin, OUTPUT);
}