#include "WifiHandler.h"

NonBlockingTimer WifiHandler::myTimer(1000);

// Define AP credentials
#define AP_SSID "demo"
//#define AP_PASSWORD ""

void WifiHandler::init()
{
    debugI("WifiHandler initialized");
    
    GfxHandler::printMessage("WifiHandler initialized");

    String ssid, password;

    // WiFi.mode(WIFI_STA);
    WiFi.mode(WIFI_AP_STA); // Set WiFi mode to both Station and Access Point

    // Configure Access Point
    //bool apSuccess = WiFi.softAP(AP_SSID, AP_PASSWORD);
    bool apSuccess = WiFi.softAP(AP_SSID);
    
    if (apSuccess)
    {
        IPAddress AP_IP = WiFi.softAPIP();
        debugI("AP Started: %s", AP_SSID);
        debugI("AP IP: %s", AP_IP.toString().c_str());
        GfxHandler::printMessage("AP: " + String(AP_SSID));
        GfxHandler::printMessage("AP IP: " + AP_IP.toString());
    }
    else
    {
        debugE("Failed to start AP!");
        GfxHandler::printMessage("Failed to start AP!");
    }

    if (PreferencesHandler::getValue("wifi_ssid", ssid) && PreferencesHandler::getValue("wifi_password", password))
    {
        WiFi.begin(ssid.c_str(), password.c_str());
        while (WiFi.status() != WL_CONNECTED)
        {
            if (myTimer.isReady())
            {
                debugI("Connecting to WiFi...");
                GfxHandler::printMessage("Connecting to WiFi...");
            }
        }
    }
    else
    {
        debugE("No wifi credentials!");
        GfxHandler::printMessage("No wifi credentials!");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        debugI("IP: %s", WiFi.localIP().toString().c_str());
        GfxHandler::printMessage("IP: " + WiFi.localIP().toString());
        Debug.begin(HOST_NAME);
    }
    else
    {
        debugE("No WiFi!");
        GfxHandler::printMessage("No WiFi!");
    }
}

void WifiHandler::loop()
{
    // debugD("WifiHandler loop");
}
