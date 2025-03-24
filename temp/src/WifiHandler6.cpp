#include "WifiHandler.h"

static NonBlockingTimer myTimer(5000);
static DNSServer dnsServer;
const char *filePath = "/wifi_networks.json";
static String ssid, password;
static IPAddress apIP(192, 168, 4, 1);
static IPAddress netMsk(255, 255, 255, 0);

void WifiHandler::init()
{
    // WiFi.mode(WIFI_STA);
    // WiFi.mode(WIFI_AP);
    // WiFi.mode(WIFI_AP_STA);
    debugI("WifiHandler initialized");
    GfxHandler::printMessage("WifiHandler initialized");
    scanAndSaveNetworks(filePath);
    connectToWifi();
}

void WifiHandler::loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        dnsServer.processNextRequest();
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        if (myTimer.isReady())
        {
            // Suspect this could be interfering with webserial if debugger is sending messages to serial
            // debugW("Lost WiFi connection!");
            // GfxHandler::printMessage("WiFi disconnected!");
        }
    }
}

void WifiHandler::connectToWifi()
{
    PreferencesHandler::getValue("wifi_ssid", ssid, SECURE_WIFI_SSID);
    PreferencesHandler::getValue("wifi_password", password, SECURE_WIFI_PASSWORD);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

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
        Debug.begin(HOST_NAME);
        initializeMDNS();
    }
    else
    {
        debugE("Failed to connect to WiFi!");
        GfxHandler::printMessage("Failed to connect to WiFi 2!");
        startAccessPoint();
    }
}

void WifiHandler::startAccessPoint()
{
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(HOST_NAME))
    {
        WiFi.softAPConfig(apIP, apIP, netMsk);
        debugI("Access Point started. SSID: %s", HOST_NAME);
        GfxHandler::printMessage("Access Point started. SSID: " + String(HOST_NAME));
        debugI("AP IP: %s", apIP.toString().c_str());
        GfxHandler::printMessage("AP IP: " + apIP.toString());
    }
    else
    {
        debugE("Failed to start Access Point!");
        GfxHandler::printMessage("Failed to start AP!");
    }

    // TODO see why this is not working
    dnsServer.start(53, "*", WiFi.softAPIP());
    debugI("DNS Server started to route all traffic to AP IP");
}

void WifiHandler::initializeMDNS()
{
    if (!MDNS.begin(HOST_NAME))
    {
        debugE("Error setting up mDNS responder!");
    }
    else
    {
        debugI("mDNS responder started. Hostname: %s.local", HOST_NAME);
        MDNS.addService("http", "tcp", 80);
    }
}

void WifiHandler::scanAndSaveNetworks(const char *filePath)
{
    if (!LittleFS.begin(true))
    {
        debugE("Failed to mount LittleFS");
        return;
    }
    debugI("LittleFS mounted successfully");

    // Start a synchronous Wi-Fi scan
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