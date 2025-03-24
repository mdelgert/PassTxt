#include "ServeDevice.h"

static NonBlockingTimer delayTimer(1000);

void ServeDevice::registerEndpoints(AsyncWebServer &server)
{
    handleDeviceInfo(server);
    handleDeviceReboot(server);
    handleDeviceWifiNetworks(server);
}

void ServeDevice::handleDeviceInfo(AsyncWebServer &server)
{
    server.on("/device/info", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        debugI("Serving /device/get (device info)");

        JsonDocument doc;
        doc["chipModel"]   = ESP.getChipModel();
        doc["chipRevision"] = (int)ESP.getChipRevision();
        doc["chipId"]      = ESP.getEfuseMac();
        doc["chipCores"] = ESP.getChipCores();
        doc["flashSize"]   = ESP.getFlashChipSize();
        doc["flashSpeed"]  = ESP.getFlashChipSpeed();
        doc["flashMode"]   = ESP.getFlashChipMode();
        doc["cpuFreqMHz"]  = ESP.getCpuFreqMHz();
        doc["sdkVersion"]  = ESP.getSdkVersion();
        doc["heapSize"]    = ESP.getHeapSize();
        doc["freeHeap"]    = ESP.getFreeHeap();
        doc["psramSize"]   = ESP.getPsramSize();
        doc["freePsram"]   = ESP.getFreePsram();
        doc["ssid"]        = WiFi.SSID();
        doc["ip"]          = WiFi.localIP().toString();
        doc["mac"]         = WiFi.macAddress();
        doc["rssi"]        = WiFi.RSSI();

        WebHandler::sendSuccessResponse(request, "GET /device/get", &doc); });
}

void ServeDevice::handleDeviceReboot(AsyncWebServer &server)
{
    server.on("/device/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        debugI("Received GET request on /device/reboot");

        JsonDocument data;
        data["message"] = "Rebooting device";
        data["success"] = true;

        WebHandler::sendSuccessResponse(request, "GET /device/reboot", &data); 
        
        if (delayTimer.isReady())
        {
            ESP.restart();
        } });
}

/*
void ServeDevice::handleDeviceWifiNetworks(AsyncWebServer &server)
{
    server.on("/device/wifi/networks", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  debugI("Serving /device/wifi/networks");

                  int networkCount = WiFi.scanNetworks(false); // Blocking call
                  //int networkCount = WiFi.scanNetworks(true);

                  debugI("Scanning for Wi-Fi networks...");

                  if (networkCount == 0)
                  {
                      WebHandler::sendErrorResponse(request, 500, "No networks found");
                      return;
                  }

                  debugI("Found %d networks", networkCount);

                  // Create a JSON document to hold the network list
                  JsonDocument doc; // Use JsonDocument for dynamic sizing
                  JsonArray networks = doc["networks"].to<JsonArray>();

                  //This will crash the application should look into saving the networks to a file at boot
                  for (int i = 0; i < networkCount; i++)
                  {
                      JsonObject network = networks.add<JsonObject>();
                      network["ssid"] = WiFi.SSID(i);
                      network["rssi"] = WiFi.RSSI(i);
                      //network["encryptionType"] = WiFi.encryptionType(i);
                      //network["isHidden"] = WiFi.SSID(i).isEmpty(); // Example logic for hidden networks
                  }

                  WebHandler::sendSuccessResponse(request, "GET /device/wifi/networks", &doc);
                  // Clean up after scan
                  WiFi.scanDelete(); // Free memory used by the scan
              });
}
*/

void ServeDevice::handleDeviceWifiNetworks(AsyncWebServer &server)
{
    server.on("/device/wifi/networks", HTTP_GET, [](AsyncWebServerRequest *request) {
        const char *filePath = "/wifi_networks.json";

        if (!LittleFS.begin(true)) {
            debugE("Failed to mount LittleFS");
            WebHandler::sendErrorResponse(request, 500, "Failed to mount filesystem");
            return;
        }

        // Open the file for reading
        File file = LittleFS.open(filePath, "r");
        if (!file) {
            debugE("Failed to open file %s for reading", filePath);
            WebHandler::sendErrorResponse(request, 500, "Failed to open network scan results");
            return;
        }

        // Read the JSON file
        JsonDocument doc; // Adjust size based on file contents
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            debugE("Failed to parse JSON from file %s", filePath);
            WebHandler::sendErrorResponse(request, 500, "Failed to parse network scan results");
            return;
        }

        // Send the JSON response
        WebHandler::sendSuccessResponse(request, "GET /device/wifi/networks", &doc);
    });
}
