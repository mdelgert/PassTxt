#ifdef ENABLE_WEB_HANDLER

#include "ServeDevice.h"
#include "Globals.h"
#include "WebHandler.h"
#include "TimeHandler.h"
#include "Timezones.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>

static NonBlockingTimer delayTimer(1000);

void ServeDevice::registerEndpoints(AsyncWebServer &server)
{
    handleDeviceInfo(server);
    handleDeviceReboot(server);
    handleDeviceWifiNetworks(server);
    handleDeviceTimezones(server);
    //handleDeviceBackup(server);
    handleDeviceFormat(server);
    handleDeviceOTA(server);
}

void ServeDevice::handleDeviceInfo(AsyncWebServer &server)
{
    server.on("/device/info", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        
        debugV("Serving /device/get");

        JsonDocument doc;

        doc["firmwareVersion"]   = SOFTWARE_VERSION;
        doc["deviceName"]   = settings.device.name;

        // doc["timezoneSettings"]    = settings.timezone;
        // doc["timezoneTimehandler"]    = TimeHandler::getCurrentTimezone();

        doc["timezone"]    = settings.device.timezone;

        //Need to cleanup when clock was not set caused reboot loop
        doc["bootCount"]    = settings.device.bootCount;

        doc["upTime"]    = settings.device.upTime;

        //doc["bootTime"] = settings.bootTime;
        time_t bootTime = settings.device.bootTime; // Previously saved timestamp
        String formattedBootTime = TimeHandler::formatDateTime("%I:%M:%S %p %m-%d-%Y", bootTime);
        
        doc["bootTime"] = formattedBootTime;
        doc["currentTime"]  = TimeHandler::formatDateTime("%I:%M:%S %p %m-%d-%Y");

        // doc["currentTime"]  = TimeHandler::formatDateTime("%I:%M:%S %p");
        // doc["currentDate"]  = TimeHandler::formatDateTime("%m-%d-%Y");

        doc["ssid"]        = WiFi.SSID();
        doc["ip"]          = WiFi.localIP().toString();
        doc["mac"]         = WiFi.macAddress();
        doc["rssi"]        = WiFi.RSSI();
        doc["wifiMode"]    = WiFi.getMode();
        doc["wifiChannel"] = WiFi.channel();
        doc["tickRateHz"] = configTICK_RATE_HZ;
        doc["chipModel"]   = ESP.getChipModel();
        doc["chipRevision"] = (int)ESP.getChipRevision();
        doc["chipId"]      = ESP.getEfuseMac();
        doc["chipCores"] = ESP.getChipCores();
        doc["heapSize"]    = ESP.getHeapSize();
        doc["freeHeap"]    = ESP.getFreeHeap();
        doc["flashSize"]   = ESP.getFlashChipSize();
        doc["flashSpeed"]  = ESP.getFlashChipSpeed();
        doc["flashMode"]   = ESP.getFlashChipMode();
        doc["cpuFreqMHz"]  = ESP.getCpuFreqMHz();
        doc["sdkVersion"]  = ESP.getSdkVersion();
        doc["psramSize"]   = ESP.getPsramSize();
        doc["freePsram"]   = ESP.getFreePsram();
        doc["lastResetTime"] = esp_timer_get_time(); // Example: microseconds since startup
        
        // LittleFS Info
        /*
        if (LittleFS.begin(true)) { // Format if mounting fails blocking exsisting file manager
            doc["littleFsTotalSpace"] = LittleFS.totalBytes();       // Total flash allocated for LittleFS
            doc["littleFsUsedSpace"] = LittleFS.usedBytes();         // Space already used in LittleFS
            doc["littleFsFreeSpace"] = LittleFS.totalBytes() - LittleFS.usedBytes(); // Available space
            LittleFS.end();
        } else {
            doc["littleFsError"] = "Failed to mount LittleFS";
        }
        */

        doc["littleFsTotalSpace"] = LittleFS.totalBytes();
        doc["littleFsUsedSpace"] = LittleFS.usedBytes();
        doc["littleFsFreeSpace"] = LittleFS.totalBytes() - LittleFS.usedBytes();
       
         // Add reset reason
        esp_reset_reason_t resetReason = esp_reset_reason();
        switch (resetReason) {
            case ESP_RST_POWERON: doc["resetReason"] = "Power-on Reset"; break;
            case ESP_RST_EXT:     doc["resetReason"] = "External Reset"; break;
            case ESP_RST_SW:      doc["resetReason"] = "Software Reset"; break;
            case ESP_RST_PANIC:   doc["resetReason"] = "Panic Reset"; break;
            case ESP_RST_INT_WDT: doc["resetReason"] = "Interrupt Watchdog Reset"; break;
            case ESP_RST_TASK_WDT:doc["resetReason"] = "Task Watchdog Reset"; break;
            case ESP_RST_WDT:     doc["resetReason"] = "Other Watchdog Reset"; break;
            case ESP_RST_DEEPSLEEP: doc["resetReason"] = "Deep Sleep Reset"; break;
            case ESP_RST_BROWNOUT: doc["resetReason"] = "Brownout Reset"; break;
            case ESP_RST_SDIO:    doc["resetReason"] = "SDIO Reset"; break;
            default:              doc["resetReason"] = "Unknown Reset"; break;
        }

        // Adding handler status
        // doc["enableWifiHandler"] = ENABLE_WIFI_HANDLER;
        // doc["enableWebHandler"] = ENABLE_WEB_HANDLER;
        // doc["enableImprovHandler"] = ENABLE_IMPROV_HANDLER;
        // doc["enableBluetoothHandler"] = ENABLE_BLUETOOTH_HANDLER;
        // doc["enableGfxHandler"] = ENABLE_GFX_HANDLER;
        // doc["enableButtonHandler"] = ENABLE_BUTTON_HANDLER;
        // doc["enableLedHandler"] = ENABLE_LED_HANDLER;
        // doc["enableRemoteDebugHandler"] = ENABLE_REMOTE_DEBUG_HANDLER;
        // doc["enableMqttHandler"] = ENABLE_MQTT_HANDLER;
        // doc["enableEzTimeHandler"] = ENABLE_EZTIME_HANDLER;

        WebHandler::sendSuccessResponse(request, "GET /device/get", &doc); });
}

void ServeDevice::handleDeviceReboot(AsyncWebServer &server)
{
    server.on("/device/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        
        debugV("Received GET request on /device/reboot");

        JsonDocument data;
        data["message"] = "Rebooting device";
        data["success"] = true;

        WebHandler::sendSuccessResponse(request, "GET /device/reboot", &data); 
        
        if (delayTimer.isReady())
        {
            ESP.restart();
        } });
}

void ServeDevice::handleDeviceWifiNetworks(AsyncWebServer &server)
{
    server.on("/device/wifi/networks", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        //const char *filePath = "/wifi_networks.json";

        if (!LittleFS.begin(true)) {
            debugE("Failed to mount LittleFS");
            WebHandler::sendErrorResponse(request, 500, "Failed to mount filesystem");
            return;
        }

        File file = LittleFS.open(WIFI_NETWORKS_FILE, "r");
        if (!file) {
            debugE("Failed to open file %s for reading", WIFI_NETWORKS_FILE);
            WebHandler::sendErrorResponse(request, 500, "Failed to open network scan results");
            return;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            debugE("Failed to parse JSON from file %s", WIFI_NETWORKS_FILE);
            WebHandler::sendErrorResponse(request, 500, "Failed to parse network scan results");
            return;
        }

        WebHandler::sendSuccessResponse(request, "GET /device/wifi/networks", &doc); });
}

void ServeDevice::handleDeviceTimezones(AsyncWebServer &server)
{
    server.on("/data/timezones", HTTP_GET, [](AsyncWebServerRequest *request){
        if (!LittleFS.begin(true)) {
            debugE("Failed to mount LittleFS");
            WebHandler::sendErrorResponse(request, 500, "Failed to mount filesystem");
            return;
        }

        File file = LittleFS.open(TIMEZONES_FILE, "r");
        if (!file) {
            debugE("Failed to open file %s for reading", TIMEZONES_FILE);
            WebHandler::sendErrorResponse(request, 500, "Failed to open timezones file");
            return;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            debugE("Failed to parse JSON from file %s", TIMEZONES_FILE);
            WebHandler::sendErrorResponse(request, 500, "Failed to parse timezones");
            return;
        }

        WebHandler::sendSuccessResponse(request, "GET /device/timezones", &doc); });
}

/*
void ServeDevice::handleDeviceTimezones(AsyncWebServer &server)
{
    server.on("/device/timezones", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument doc;  // Use DynamicJsonDocument
        JsonArray array = doc.to<JsonArray>();

        for (size_t i = 0; i < tz_db_tzs_size; i++) {  // Use indexed loop
            const tz_db_pair_t &tz = tz_db_tzs[i];  // Access each timezone
            JsonObject obj = array.add<JsonObject>(); // Use add<JsonObject>()
            obj["name"] = tz.name;
            obj["posix_str"] = tz.posix_str;
        }

        WebHandler::sendSuccessResponse(request, "GET /device/timezones", &doc);
    });
}
*/

/*
// Tar not functioning at this time zipping files on frontend
void ServeDevice::handleDeviceBackup(AsyncWebServer &server)
{
    server.on("/device/backup", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  debugV("Received GET request on /device/backup");

                  File exportFile = LittleFS.open("/backup.tar", "w");
                  if (!exportFile)
                  {
                      request->send(500, "text/plain", "Failed to create export file");
                      return;
                  }

                  File root = LittleFS.open("/");
                  File file = root.openNextFile();

                  while (file)
                  {
                      // Write file metadata
                      String header = "FILE:" + String(file.name()) + "\nSIZE:" + String(file.size()) + "\n";
                      exportFile.write((const uint8_t *)header.c_str(), header.length());

                      // Write file content
                      uint8_t buffer[512];
                      while (file.available())
                      {
                          size_t bytesRead = file.read(buffer, sizeof(buffer));
                          exportFile.write(buffer, bytesRead);
                      }

                      // exportFile.write("\nEND\n", 5); // Mark the end of the file
                      exportFile.write(reinterpret_cast<const uint8_t *>("\nEND\n"), 5); // Mark the end of the file
                      file = root.openNextFile();
                  }

                  exportFile.close();
                  // request->send(200, "text/plain", "Backup exported successfully to /backup.tar");

                  // Create a JSON response
                  JsonDocument data;
                  data["data"] = "Backup success";

                  WebHandler::sendSuccessResponse(request, "GET /device/backup", &data); });
}
*/

void ServeDevice::handleDeviceFormat(AsyncWebServer &server)
{
    server.on("/device/format", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        debugV("Received GET request on /device/format");

        JsonDocument data;
        data["message"] = "Formating device";
        data["success"] = true;

        WebHandler::sendSuccessResponse(request, "GET /device/format", &data);

        // Format LittleFS
        if (LittleFS.format())
        {
            debugI("LittleFS formatted successfully!");
        }
        else
        {
            debugE("Failed to format LittleFS.");
        }

        });
}

void ServeDevice::handleDeviceOTA(AsyncWebServer &server)
{
    server.on("/device/ota", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (Update.hasError()) {
            JsonDocument data;
            data["message"] = "Update failed!";
            data["success"] = false;
            String response;
            serializeJson(data, response);
            WebHandler::sendErrorResponse(request, 500, response.c_str());
        } else {
            JsonDocument data;
            data["message"] = "Update successful! Rebooting...";
            data["success"] = true;
            String response;
            serializeJson(data, response);
            WebHandler::sendSuccessResponse(request, "POST /device/ota", &data);
            delay(1000);
            ESP.restart();
        }
    }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (index == 0) {
            debugV("Update Start: %s", filename.c_str());
            if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
                Update.printError(Serial);
            }
        }
        if (!Update.hasError()) {
            if (Update.write(data, len) != len) {
                Update.printError(Serial);
            }
        }
        if (final) {
            if (Update.end(true)) {
                debugV("Update Success: %u bytes", index + len);
            } else {
                Update.printError(Serial);
            }
        }
    });
}

#endif // ENABLE_WEB_HANDLER