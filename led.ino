

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
