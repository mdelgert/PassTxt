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

    GfxHandler::printMessage("SSID: " + ssid);
    delay(3000);

    GfxHandler::printMessage("Password: " + password);
    delay(3000);

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

    // if (hasSsid && hasPassword)
    // {
    //     while (WiFi.status() != WL_CONNECTED)
    //     {
    //         if (myTimer.isReady())
    //         {
    //             debugI("Connecting to WiFi...");
    //             GfxHandler::printMessage("Connecting to WiFi...");
    //         }
    //     }
    // }
    // else
    // {
    //     debugE("No valid WiFi credentials! Using defaults.");
    //     GfxHandler::printMessage("No valid WiFi credentials!");
    // }

    if (WiFi.status() == WL_CONNECTED)
    {
        String ipAddress = WiFi.localIP().toString();
        debugI("IP: %s", ipAddress.c_str());
        GfxHandler::printMessage("IP: " + ipAddress);
        Debug.begin(HOST_NAME);
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
