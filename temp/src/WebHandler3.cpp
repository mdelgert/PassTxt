#include "WebHandler.h"

NonBlockingTimer WebHandler::myTimer(1000);
AsyncWebServer WebHandler::server(80);
static String ssid, password;
//const char* apiToken = "test";

bool WebHandler::isTokenValid(AsyncWebServerRequest *request) {
    if (request->hasHeader("Authorization")) {
        AsyncWebHeader* header = request->getHeader("Authorization");
        // Todo - add value to preferences and implement
        if (String(header->value()) == String("Bearer ") + API_TOKEN) {
            return true;
        }
    }
    return false;
}

void WebHandler::addCorsHeaders(AsyncWebServerResponse *response)
{
    if (ENABLE_SECURE_CORS)
        return; // Only enable this for testing not safe for production
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
}

void WebHandler::serveEmbeddedFile(const char *path, const uint8_t *start, const uint8_t *end, const char *contentType)
{
    server.on(path, HTTP_GET, [path, start, end, contentType](AsyncWebServerRequest *request)
              {
        size_t fileSize = end - start;
        AsyncWebServerResponse* response = request->beginResponse_P(200, contentType, start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response);
        debugI("Served: %s", path); });
}

void WebHandler::init()
{
    if(!ENABLE_WEBHANDLER) return;
    
    debugI("WebHandler initialized");

    while (WiFi.status() != WL_CONNECTED)
    {
        if (myTimer.isReady())
        {
            debugI("WebHandler waiting for WiFi...");
            break;
        }
    }

    // Initialize LittleFS
    if (!LittleFS.begin(true))
    {
        debugE("Failed to mount LittleFS");
        return;
    }
    debugI("LittleFS mounted successfully");

    serveRoot();
    serveSettings();
    serveActions();
    serveNotFound();
    serveDeviceInfo();
    serveWifiNetworks();
    serveWifiGet();
    serveWifiSave();
    serveReboot();
    serveSecure();

    // Start the web server
    server.begin();
    debugI("Web server started!");
}

void WebHandler::loop()
{
    // debugI("WebHandler loop");
}

void WebHandler::serveRoot()
{
    serveEmbeddedFile("/actions.html", actions_html_start, actions_html_end, "text/html");
    serveEmbeddedFile("/actions.js", actions_js_start, actions_js_end, "application/javascript");
    serveEmbeddedFile("/advanced.html", advanced_html_start, advanced_html_end, "text/html");
    serveEmbeddedFile("/advanced.js", advanced_js_start, advanced_js_end, "application/javascript");
    serveEmbeddedFile("/footer.html", footer_html_start, footer_html_end, "text/html");
    serveEmbeddedFile("/global.js", global_js_start, global_js_end, "application/javascript");
    serveEmbeddedFile("/", index_html_start, index_html_end, "text/html");
    serveEmbeddedFile("/index.js", index_js_start, index_js_end, "application/javascript");
    serveEmbeddedFile("/navbar.html", navbar_html_start, navbar_html_end, "text/html");
    serveEmbeddedFile("/settings.html", settings_html_start, settings_html_end, "text/html");
    serveEmbeddedFile("/settings.js", settings_js_start, settings_js_end, "application/javascript");
    serveEmbeddedFile("/styles.css", styles_css_start, styles_css_end, "text/css");
    debugI("WebHandler handleRoot");
}

void WebHandler::serveSettings()
{
    // Serve files from LittleFS
    server.on("/settings/save", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              {
        debugI("Received JSON payload on /settings/save");

        JsonDocument doc;

        // Parse the JSON data
        DeserializationError error = deserializeJson(doc, data, len);
        if (error) {
            debugE("Failed to parse JSON: %s", error.c_str());
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            return;
        }

        // Convert JSON to a string
        String jsonString;
        serializeJson(doc, jsonString);

        // Save JSON to LittleFS
        File file = LittleFS.open("/settings.json", "w");
        if (!file) {
            debugE("Failed to open file for writing");
            request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save file\"}");
            return;
        }
        file.print(jsonString);
        file.close();

        debugI("JSON saved to /settings.json");
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\",\"message\":\"File saved successfully\"}");
        WebHandler::addCorsHeaders(response);
        request->send(response); });

    // Serve JSON data
    server.on("/settings/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        File file = LittleFS.open("/settings.json", "r");
        if (!file) {
            debugE("Failed to open /settings.json");
            AsyncWebServerResponse *response = request->beginResponse(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
            WebHandler::addCorsHeaders(response);
            request->send(response);
            return;
        }

        String json = file.readString();
        file.close();
        debugI("Serving /settings.json");

        JsonDocument doc; // Use the simplified declaration
        DeserializationError error = deserializeJson(doc, json);
        if (error) {
            debugE("Failed to parse saved JSON: %s", error.c_str());
            request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Internal server error\"}");
            return;
        }

        // Convert JSON back to string for response
        String responseString;
        serializeJson(doc, responseString);

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", responseString);
        WebHandler::addCorsHeaders(response);
        request->send(response); });

    debugI("WebHandler serveSettings");
}

void WebHandler::serveActions()
{
    // Serve files from LittleFS
    server.on("/actions/save", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              {
        debugI("Received JSON payload on /actions/save");

        JsonDocument doc;

        // Parse the JSON data
        DeserializationError error = deserializeJson(doc, data, len);
        if (error) {
            debugE("Failed to parse JSON: %s", error.c_str());
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            return;
        }

        // Convert JSON to a string
        String jsonString;
        serializeJson(doc, jsonString);

        // Save JSON to LittleFS
        File file = LittleFS.open("/actions.json", "w");
        if (!file) {
            debugE("Failed to open file for writing");
            request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save file\"}");
            return;
        }
        file.print(jsonString);
        file.close();

        debugI("JSON saved to /actions.json");
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\",\"message\":\"File saved successfully\"}");
        WebHandler::addCorsHeaders(response);
        request->send(response); });

    // Serve JSON data
    server.on("/actions/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        File file = LittleFS.open("/actions.json", "r");
        if (!file) {
            debugE("Failed to open /actions.json");
            AsyncWebServerResponse *response = request->beginResponse(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
            WebHandler::addCorsHeaders(response);
            request->send(response);
            return;
        }

        String json = file.readString();
        file.close();
        debugI("Serving /actions.json");

        JsonDocument doc; // Use the simplified declaration
        DeserializationError error = deserializeJson(doc, json);
        if (error) {
            debugE("Failed to parse saved JSON: %s", error.c_str());
            request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Internal server error\"}");
            return;
        }

        // Convert JSON back to string for response
        String responseString;
        serializeJson(doc, responseString);

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", responseString);
        WebHandler::addCorsHeaders(response);
        request->send(response); });

    debugI("WebHandler serveActions");
}

void WebHandler::serveNotFound()
{
    // Handle preflight CORS requests
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
        if (request->method() == HTTP_OPTIONS) {
            AsyncWebServerResponse *response = request->beginResponse(204); // No Content
            WebHandler::addCorsHeaders(response);
            request->send(response);
            debugI("Handled CORS preflight request");
        } else {
            request->send(404, "text/plain", "Not found");
        } });
}

void WebHandler::serveDeviceInfo()
{
    server.on("/device/get", HTTP_GET, [](AsyncWebServerRequest *request) {
        debugI("Serving /device/get (device info)");

        // Create a JSON document for device info.
        // You can size this based on how much data you're storing.
        JsonDocument doc;

        // Basic chip info
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
        doc["rssi"]        = WiFi.RSSI(); // WiFi signal strength

        // Convert JSON doc to string for response
        String responseString;
        serializeJson(doc, responseString);

        // Create and send the response with any CORS headers if needed
        AsyncWebServerResponse *response = request->beginResponse(
            200, "application/json", responseString);
        WebHandler::addCorsHeaders(response);  // If you want your feature flag or logic, do it here
        request->send(response);
    });
}

void WebHandler::serveWifiNetworks()
{
    server.on("/wifi/networks/get", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        debugI("Scanning for Wi-Fi networks...");

        int networkCount = WiFi.scanNetworks();
        if (networkCount == -1)
        {
            debugE("Wi-Fi scan failed");
            AsyncWebServerResponse *response = request->beginResponse(
                500, "application/json", "{\"status\":\"error\",\"message\":\"Wi-Fi scan failed\"}");
            WebHandler::addCorsHeaders(response);
            request->send(response);
            return;
        }

        debugI("Found %d networks", networkCount);

        // Create a JSON document to hold the network list
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

        String responseString;
        serializeJson(doc, responseString);

        AsyncWebServerResponse *response = request->beginResponse(
            200, "application/json", responseString);
        WebHandler::addCorsHeaders(response);
        request->send(response);

        debugI("Served /wifi/get");
    });
}

void WebHandler::serveWifiGet()
{
    server.on("/wifi/get", HTTP_GET, [](AsyncWebServerRequest *request) {
        debugI("Serving /wifi/get");

        // Create a JSON document for device info.
        // You can size this based on how much data you're storing.
        JsonDocument doc;

        PreferencesHandler::getValue("wifi_ssid", ssid, SECURE_WIFI_SSID);
        PreferencesHandler::getValue("wifi_password", password, SECURE_WIFI_PASSWORD);

        // Basic chip info
        doc["wifi_ssid"]   = ssid;
        doc["wifi_password"] = password;

        // Convert JSON doc to string for response
        String responseString;
        serializeJson(doc, responseString);

        // Create and send the response with any CORS headers if needed
        AsyncWebServerResponse *response = request->beginResponse(
            200, "application/json", responseString);
        WebHandler::addCorsHeaders(response);  // If you want your feature flag or logic, do it here
        request->send(response);
    });
}

void WebHandler::serveWifiSave()
{
    server.on("/wifi/save", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              {
        debugI("Received JSON payload on /wifi/save");

        // Parse the JSON data
        JsonDocument doc; // Use JsonDocument for dynamic sizing
        DeserializationError error = deserializeJson(doc, data, len);
        if (error) {
            debugE("Failed to parse JSON: %s", error.c_str());
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            return;
        }

        // Extract Wi-Fi credentials
        const char *ssid = doc["wifi_ssid"] | "";
        const char *password = doc["wifi_password"] | "";

        if (strlen(ssid) == 0 || strlen(password) == 0) {
            debugE("Wi-Fi credentials missing");
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing Wi-Fi credentials\"}");
            return;
        }

        // Save credentials to preferences
        PreferencesHandler::setValue("wifi_ssid", String(ssid));
        PreferencesHandler::setValue("wifi_password", String(password));

        debugI("Wi-Fi credentials saved: SSID=%s", ssid);

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\",\"message\":\"Wi-Fi credentials saved\"}");
        WebHandler::addCorsHeaders(response);
        request->send(response); });

    debugI("WebHandler serveWifiSave");
}

void WebHandler::serveReboot()
{
    server.on("/device/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        debugI("Reboot endpoint triggered");

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\",\"message\":\"Device rebooting\"}");
        WebHandler::addCorsHeaders(response);
        request->send(response);

        debugI("Rebooting device...");
        delay(1000); // Short delay to allow response to be sent
        ESP.restart();
    });

    debugI("WebHandler handleReboot");
}

void WebHandler::serveSecure()
{
    server.on("/secure", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isTokenValid(request)) {
                request->send(403, "text/plain", "Forbidden: Invalid token.");
                return;
            }
            request->send(200, "text/plain", "Authorized: Token is valid.");
        });
}