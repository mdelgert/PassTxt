#include "WifiHandler.h"

NonBlockingTimer WifiHandler::myTimer(1000);

void WifiHandler::init()
{
    debugI("WifiHandler initialized");
    GfxHandler::printMessage("WifiHandler initialized");

    String ssid;
    String password;

    // Retrieve SSID with fallback to Secure.h
    PreferencesHandler::getValue("wifi_ssid", ssid, SECURE_WIFI_SSID);
    PreferencesHandler::getValue("wifi_password", password, SECURE_WIFI_PASSWORD);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED)
    {
        if (myTimer.isReady())
        {
            debugI("Connecting to WiFi...");
            GfxHandler::printMessage("Connecting to WiFi...");
        }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        String ipAddress = WiFi.localIP().toString();
        debugI("IP: %s", ipAddress.c_str());
        GfxHandler::printMessage("IP: " + ipAddress);
        Debug.begin(HOST_NAME);

        // Initialize mDNS
        if (!MDNS.begin(HOST_NAME)) // HOST_NAME should be a defined string, e.g., "mydevice"
        {
            debugE("Error setting up mDNS responder!");
            //GfxHandler::printMessage("Error setting up mDNS responder!");
        }
        else
        {
            debugI("mDNS responder started. Hostname: %s.local", HOST_NAME);
            //GfxHandler::printMessage("mDNS: " + String(HOST_NAME) + ".local");
        }

        // Optional: Add service to mDNS
        MDNS.addService("http", "tcp", 80); // Example for an HTTP server
    }
    else
    {
        debugE("Failed to connect to WiFi!");
        GfxHandler::printMessage("Failed to connect to WiFi!");
    }
}

void WifiHandler::loop()
{
    // debugD("WifiHandler loop");
}
