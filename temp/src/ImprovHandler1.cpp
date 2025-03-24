#include "Globals.h"
#include "ImprovHandler.h"
#include "PreferencesHandler.h"
#include "GfxHandler.h"

ImprovWiFi ImprovHandler::improvSerial(&Serial);
WiFiServer ImprovHandler::server(80); // HTTP server on port 80
NonBlockingTimer ImprovHandler::myTimer(1000);

void ImprovHandler::init()
{
    debugD("Initializing ImprovHandler");

    WiFi.mode(WIFI_STA);
    debugI("WiFi set to station mode");

    WiFi.disconnect();
    debugI("WiFi disconnected");

    String ssid, password;

    // Attempt to retrieve saved Wi-Fi credentials
    if (PreferencesHandler::getValue("wifi_ssid", ssid) && PreferencesHandler::getValue("wifi_password", password))
    {
        debugI("Retrieved Wi-Fi credentials from preferences. SSID: %s", ssid.c_str());

        WiFi.begin(ssid.c_str(), password.c_str());

        // Wait for connection or timeout
        unsigned long startAttemptTime = millis();
        const unsigned long connectTimeout = 10000; // 10 seconds

        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < connectTimeout)
        {
            debugD("Attempting to connect to Wi-Fi...");
            delay(500);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Debug.begin(HOST_NAME);
            debugI("Successfully connected to Wi-Fi. SSID: %s", ssid.c_str());
            server.begin(); // Start HTTP server
            debugI("HTTP server started");
            // return;
        }
        else
        {
            debugE("Failed to connect to Wi-Fi. Falling back to Improv setup.");
        }
    }
    else
    {
        debugW("No Wi-Fi credentials found in preferences. Waiting for Improv setup.");
    }

    // Initialize ImprovWiFi setup if no valid connection
    improvSerial.setDeviceInfo(ImprovTypes::ChipFamily::CF_ESP32_S3, "ImprovWiFiLib", SOFTWARE_VERSION, "Demo", "http://{LOCAL_IPV4}?name=Guest");
    improvSerial.onImprovError(onImprovWiFiErrorCb);
    improvSerial.onImprovConnected(onImprovWiFiConnectedCb);
    //improvSerial.setCustomConnectWiFi(connectWifi); // Optional

    debugD("ImprovWiFi setup initialized");
}

void ImprovHandler::loop()
{
    improvSerial.handleSerial();

    if (improvSerial.isConnected())
    {
        handleHttpRequest();
    }

    if (myTimer.isReady())
    {
        GfxHandler::printMessage("IP: " + WiFi.localIP().toString());
    }
}

bool ImprovHandler::connectWifi(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);

    while (!improvSerial.isConnected())
    {
        LEDHandler::setColor(CRGB::Red);
    }

    LEDHandler::setColor(CRGB::Green);

    return true;
}

void ImprovHandler::onImprovWiFiErrorCb(ImprovTypes::Error err)
{
    server.stop();
    debugE("ImprovWiFi error occurred: %d", err);
}

void ImprovHandler::onImprovWiFiConnectedCb(const char *ssid, const char *password)
{
    Debug.begin(HOST_NAME);
    server.begin();

    debugI("ImprovWiFi connected to SSID: %s", ssid);

    PreferencesHandler::setValue("wifi_ssid", String(ssid));
    PreferencesHandler::setValue("wifi_password", String(password));

    debugD("WiFi credentials saved to preferences. SSID: %s", ssid);
}

void ImprovHandler::handleHttpRequest()
{
    WiFiClient client = server.available();
    if (client)
    {
        char linebuf[512] = {0}; // Buffer for reading HTTP request
        size_t charcount = 0;

        boolean currentLineIsBlank = true;
        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                if (charcount < sizeof(linebuf) - 1)
                {
                    linebuf[charcount++] = c;
                }

                // Detect end of HTTP request
                if (c == '\n' && currentLineIsBlank)
                {
                    // Send HTTP response
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    client.println("<!DOCTYPE HTML><html><head>");
                    client.println("<style>");
                    client.println("body { background-color: #121212; color:rgb(142, 142, 142); font-family: Arial, sans-serif; margin: 0; padding: 20px; }");
                    client.println("h1, h3 { color:rgb(142, 142, 142); }");
                    client.println("ul { list-style: none; padding: 0; }");
                    client.println("li { margin: 5px 0; }");
                    client.println("a { color:rgb(142, 142, 142); text-decoration: none; }");
                    client.println("a:hover { text-decoration: underline; }");
                    client.println("#welcome { font-size: 24px; margin-bottom: 20px; }");
                    client.println("</style>");
                    client.println("</head><body>");
                    client.println("<h1 id=\"welcome\">Welcome!</h1>");
                    client.println("<p>This is a simple webpage served by your ESP32</p>");
                    client.println("<h3>Chip Info</h3>");
                    client.println("<ul><li>Model:");
                    client.println(ESP.getChipModel());
                    client.println("</li><li>Cores: ");
                    client.println(ESP.getChipCores());
                    client.println("</li><li>Revision: ");
                    client.println(ESP.getChipRevision());
                    client.println("</li></ul>");
                    client.println("<h3>Network Info</h3>");
                    client.println("<ul><li>SSID: ");
                    client.println(WiFi.SSID());
                    client.println("</li><li>IP Address: ");
                    client.println(WiFi.localIP());
                    client.println("</li><li>MAC Address: ");
                    client.println(WiFi.macAddress());
                    client.println("</li></ul>");
                    client.println("<script>const params = new URLSearchParams(document.location.search);");
                    client.println("document.getElementById('welcome').innerHTML = 'Welcome' + (params.get(\"name\") ? ', ' + params.get('name') : '') + '!';");
                    client.println("</script>");
                    client.println("</body></html>");
                    break;
                }

                if (c == '\n')
                {
                    currentLineIsBlank = true;
                }
                else if (c != '\r')
                {
                    currentLineIsBlank = false;
                }
            }
        }
        // delay(1);
        yield();
        client.stop();
        debugI("HTTP client disconnected");
    }
}