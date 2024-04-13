void waveCheck()
{
    if (waveCount >= 10)
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