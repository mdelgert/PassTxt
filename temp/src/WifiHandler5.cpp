#include "WifiHandler.h"
#include <DNSServer.h>  // Include DNSServer library

NonBlockingTimer WifiHandler::myTimer(1000);

#define ENABLE_AP_AND_STA  // Feature flag to enable both AP and WiFi STA modes

static DNSServer dnsServer;  // Define DNS server instance

void WifiHandler::init()
{
    debugI("WifiHandler initialized");
    GfxHandler::printMessage("WifiHandler initialized");

    String ssid;
    String password;

    // Retrieve SSID with fallback to Secure.h
    PreferencesHandler::getValue("wifi_ssid", ssid, SECURE_WIFI_SSID);
    PreferencesHandler::getValue("wifi_password", password, SECURE_WIFI_PASSWORD);

#ifdef ENABLE_AP_AND_STA
    debugI("Feature: Both AP and STA mode enabled.");
    startAccessPoint();
#endif

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (myTimer.isReady())
        {
            debugI("Connecting to WiFi...");
            GfxHandler::printMessage("Connecting to WiFi...");
        }

        // Timeout after 15 seconds
        if (millis() - startTime > 15000)
        {
            debugE("Failed to connect to WiFi. Starting Access Point...");
            GfxHandler::printMessage("Failed to connect to WiFi!");
            startAccessPoint();
            return;
        }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        String ipAddress = WiFi.localIP().toString();
        debugI("IP: %s", ipAddress.c_str());
        GfxHandler::printMessage("IP: " + ipAddress);
        Debug.begin(HOST_NAME);

        // Initialize mDNS
        initializeMDNS();
    }
    else
    {
        debugE("Failed to connect to WiFi!");
        GfxHandler::printMessage("Failed to connect to WiFi!");
        startAccessPoint();
    }
}

void WifiHandler::startAccessPoint()
{
    String apSSID = "MyDevice_AP"; // Customize your AP SSID
    String apPassword = "12345678"; // Optional password for secure AP

    WiFi.mode(WIFI_AP_STA); // Enable both AP and STA modes
    if (WiFi.softAP(apSSID.c_str(), apPassword.c_str()))
    {
        debugI("Access Point started. SSID: %s", apSSID.c_str());
        GfxHandler::printMessage("AP Mode: " + apSSID);
        IPAddress ip = WiFi.softAPIP();
        debugI("AP IP address: %s", ip.toString().c_str());
        GfxHandler::printMessage("AP IP: " + ip.toString());
    }
    else
    {
        debugE("Failed to start Access Point!");
        GfxHandler::printMessage("Failed to start AP!");
    }

    // Start DNS server to route DNS requests
    dnsServer.start(53, "*", WiFi.softAPIP());
    debugI("DNS Server started to route all traffic to AP IP");
}

void WifiHandler::initializeMDNS()
{
    if (!MDNS.begin(HOST_NAME)) // HOST_NAME should be a defined string, e.g., "mydevice"
    {
        debugE("Error setting up mDNS responder!");
    }
    else
    {
        debugI("mDNS responder started. Hostname: %s.local", HOST_NAME);
        MDNS.addService("http", "tcp", 80); // Example for an HTTP server
    }
}

void WifiHandler::loop()
{
    // Handle DNS server requests if in AP mode
#ifdef ENABLE_AP_AND_STA
    dnsServer.processNextRequest();
#endif

    // Optionally, monitor WiFi connection
    if (WiFi.status() != WL_CONNECTED)
    {
        debugW("Lost WiFi connection!");
        GfxHandler::printMessage("WiFi disconnected!");
        // Reconnect or stay in AP mode
    }
}
