
void sensorUpdate()
{
    int curr_distance = measure_distance();
    update_samples(curr_distance);
    delay(70);
}

void handPatternCheck()
{
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