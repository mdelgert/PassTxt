#ifdef ENABLE_WIFI_HANDLER

#include "WifiHandler.h"

static NonBlockingTimer myTimer(5000);
static DNSServer dnsServer;
const char *filePath = "/wifi_networks.json";
static IPAddress apIP(192, 168, 4, 1);
static IPAddress netMsk(255, 255, 255, 0);

void WifiHandler::init()
{
    debugI("WifiHandler initialized");
    GfxHandler::printMessage("WifiHandler initialized");
    //scanAndSaveNetworks(filePath);
    connectToWifi();
    //startAccessPoint();
}

void WifiHandler::loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {   //writes lots of messages to serial when not in AP mode
        //if implementing should check if it interferes with Improv
        //also should do if statement to check if in AP mode
        //dnsServer.processNextRequest();
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        if (myTimer.isReady())
        {   //This might be interfering with Improv because its sending serial messages at the same time
            //debugW("Lost WiFi connection!");
            GfxHandler::printMessage("WiFi disconnected!");
        }
    }
}

void WifiHandler::connectToWifi()
{
    // Must be WIFI_STA or WIFI_AP_STA mode
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.wifiSSID, settings.wifiPassword);

    while (WiFi.status() != WL_CONNECTED)
    {
        if (myTimer.isReady())
        {
            debugE("Failed to connect to WiFi. Starting Access Point...");
            GfxHandler::printMessage("Failed to connect to WiFi 1!");
            break;
        }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        String ipAddress = WiFi.localIP().toString();
        debugI("IP: %s", ipAddress.c_str());
        GfxHandler::printMessage("IP: " + ipAddress);
        RemoteDebugHandler::startNetwork();
        initializeMDNS();
    }
    else
    {
        debugE("Failed to connect to WiFi!");
        GfxHandler::printMessage("Failed to connect to WiFi 2!");
        // This will block ImprovWifi from starting
        startAccessPoint();
    }
}

void WifiHandler::startAccessPoint()
{
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(settings.deviceName))
    {
        WiFi.softAPConfig(apIP, apIP, netMsk);
        debugI("Access Point started. SSID: %s", settings.deviceName.c_str());
        GfxHandler::printMessage("Access Point started. SSID: " + settings.deviceName);
        debugI("AP IP: %s", apIP.toString().c_str());
        GfxHandler::printMessage("AP IP: " + apIP.toString());
    }
    else
    {
        debugE("Failed to start Access Point!");
        GfxHandler::printMessage("Failed to start AP!");
    }

    // TODO see why this is not working
    //dnsServer.start(53, "*", WiFi.softAPIP());
    //debugI("DNS Server started to route all traffic to AP IP");
}

void WifiHandler::initializeMDNS()
{
    MDNS.begin(settings.deviceName);  // Hostname: demo.local
    MDNS.addService("_http", "_tcp", 80);  // Advertise HTTP service on port 80
}

void WifiHandler::scanAndSaveNetworks(const char *filePath)
{
    if (settings.WifiScan == false)
    {
        debugI("Wi-Fi scan disabled");
        return;
    }
    
    if (!LittleFS.begin(true))
    {
        debugE("Failed to mount LittleFS");
        return;
    }
    debugI("LittleFS mounted successfully");

    int networkCount = WiFi.scanNetworks(false); // Blocking call
    debugI("Scanning for Wi-Fi networks...");

    if (networkCount < 0)
    {
        debugE("Wi-Fi scan failed with code %d", networkCount);
        return;
    }

    // Create a JSON document to store the network results
    JsonDocument doc; // Use JsonDocument for dynamic sizing
    JsonArray networks = doc["networks"].to<JsonArray>();

    for (int i = 0; i < networkCount; i++)
    {
        JsonObject network = networks.add<JsonObject>();
        network["ssid"] = WiFi.SSID(i);
        network["rssi"] = WiFi.RSSI(i);
        network["encryptionType"] = WiFi.encryptionType(i);
        network["isHidden"] = WiFi.SSID(i).isEmpty(); // Example logic for hidden networks
    }

    // Save the JSON document to LittleFS
    File file = LittleFS.open(filePath, "w");
    if (!file)
    {
        debugE("Failed to open file %s for writing", filePath);
        return;
    }

    if (serializeJson(doc, file) == 0)
    {
        debugE("Failed to write JSON to file %s", filePath);
    }
    else
    {
        debugI("Network scan results saved to %s", filePath);
    }

    file.close();

    // Free memory used by the scan
    WiFi.scanDelete();
}

#endif // ENABLE_WIFI_HANDLER