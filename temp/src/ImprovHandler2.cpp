#ifdef ENABLE_IMPROV_HANDLER

#include "ImprovHandler.h"
#include "ConfigManager.h"
#include "GfxHandler.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

ImprovWiFi ImprovHandler::improvSerial(&Serial);
static NonBlockingTimer myTimer(5000);
const char *filePath = "/wifi_networks.json";
static IPAddress apIP(192, 168, 4, 1);
static IPAddress netMsk(255, 255, 255, 0);

void ImprovHandler::init()
{
    GfxHandler::printMessage("ImprovHandler::init()");
    //scanAndSaveNetworks(filePath);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    improvSerial.setDeviceInfo(ImprovTypes::
                                   ChipFamily::CF_ESP32_S3,
                               "ImprovWiFi", SOFTWARE_VERSION,
                               "Demo", "http://{LOCAL_IPV4}");

    improvSerial.onImprovError(onImprovWiFiErrorCb);
    improvSerial.onImprovConnected(onImprovWiFiConnectedCb);

    if (improvSerial.tryConnectToWifi(settings.wifiSSID.c_str(), settings.wifiPassword.c_str()))
    {
        String ipAddress = WiFi.localIP().toString();
        debugI("IP: %s", ipAddress.c_str());
        GfxHandler::printMessage("IP: " + ipAddress);
        RemoteDebugHandler::startNetwork();
        MDNS.begin(settings.deviceName);      // Hostname: demo.local
        MDNS.addService("_http", "_tcp", 80); // Advertise HTTP service on port 80
    }
    else
    {
        GfxHandler::printMessage("alternativeSetupMethod");
        alternativeSetupMethod();
    }

    debugD("ImprovWiFi initialized");
}

void ImprovHandler::loop()
{
    improvSerial.handleSerial();

    /*
    if (WiFi.status() != WL_CONNECTED)
    {
        if (myTimer.isReady())
        { 
            // This might be interfering with Improv because its sending serial messages at the same time
            // debugW("Lost WiFi connection!");
            GfxHandler::printMessage("WiFi disconnected!");
        }
    }
    */
}

void ImprovHandler::onImprovWiFiErrorCb(ImprovTypes::Error err)
{
    debugE("ImprovWiFi error occurred: %d", err);
}

void ImprovHandler::onImprovWiFiConnectedCb(const char *ssid, const char *password)
{
    settings.wifiSSID = String(ssid);
    settings.wifiPassword = String(password);
    ConfigManager::save();
    debugD("WiFi credentials saved to preferences. SSID: %s", ssid);
    GfxHandler::printMessage("Rebooting...");
    debugI("Rebooting...");
    delay(1000);
    ESP.restart();
}

void ImprovHandler::alternativeSetupMethod()
{
    // Set ESP32 to Access Point mode
    WiFi.mode(WIFI_AP);

    // Start the Access Point without a password
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
}

void ImprovHandler::scanAndSaveNetworks(const char *filePath)
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

#endif // ENABLE_IMPROV_HANDLER