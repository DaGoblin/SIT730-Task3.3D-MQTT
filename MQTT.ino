
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