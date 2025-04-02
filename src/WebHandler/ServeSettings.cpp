#ifdef ENABLE_WEB_HANDLER

#include <LittleFS.h>
#include <ArduinoJson.h>
#include "ServeSettings.h"
#include "Globals.h"
#include "WebHandler.h"

void ServeSettings::registerEndpoints(AsyncWebServer &server)
{
    handleGetSettings(server);
    handleSetSettings(server);
    server.on("/settings/deprecated/get", HTTP_GET, handleGetSettingsDeprecated);
    server.on("/settings/deprecated/set", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleSetSettingsDeprecated);
}

void ServeSettings::handleGetSettings(AsyncWebServer &server)
{
    server.on("/settings/get", HTTP_GET, [](AsyncWebServerRequest *request){
        
        debugV("Received GET request on /settings/get");

        JsonDocument doc;

        // Device
        doc["device"]["name"]             = deviceConfig.getDeviceName();
        doc["device"]["timezone"]         = deviceConfig.getDeviceTimezone();
        doc["device"]["bootCommand"]      = deviceConfig.getDeviceBootCommand();
        doc["device"]["singlePress"]      = deviceConfig.getDeviceSinglePress();
        doc["device"]["doublePress"]      = deviceConfig.getDeviceDoublePress();
        doc["device"]["longPress"]        = deviceConfig.getDeviceLongPress();
        doc["device"]["userName"]         = deviceConfig.getDeviceUserName();
        doc["device"]["userPassword"]     = deviceConfig.getDeviceUserPassword();

        // WiFi
        doc["wifi"]["ssid"]               = deviceConfig.getWifiSsid();
        doc["wifi"]["password"]           = deviceConfig.getWifiPassword();
        doc["wifi"]["scan"]               = deviceConfig.getWifiScan();

        // MQTT
        doc["mqtt"]["enabled"]            = deviceConfig.getMqttEnabled();
        doc["mqtt"]["server"]             = deviceConfig.getMqttServer();
        doc["mqtt"]["port"]               = deviceConfig.getMqttPort();
        doc["mqtt"]["username"]           = deviceConfig.getMqttUsername();
        doc["mqtt"]["password"]           = deviceConfig.getMqttPassword();
        doc["mqtt"]["ssl"]                = deviceConfig.getMqttSsl();
        doc["mqtt"]["subTopic"]           = deviceConfig.getMqttSubTopic();
        doc["mqtt"]["pubTopic"]           = deviceConfig.getMqttPubTopic();

        // Security
        doc["security"]["apiKey"]         = deviceConfig.getSecurityApiKey();

        String responseBody;
        serializeJson(doc, responseBody);
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", responseBody);
        WebHandler::addCorsHeaders(response);
        request->send(response);
    });
}

void ServeSettings::handleSetSettings(AsyncWebServer &server)
{
    server.on("/settings/set", HTTP_POST, [](AsyncWebServerRequest *request) {}, nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            debugV("Received POST request on /settings/archieve/set");

            WebHandler::printRequestBody(request, data, len);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, data, len);

            if (error) {
                debugE("JSON deserialization failed: %s", error.c_str());
                WebHandler::sendErrorResponse(request, 400, "Invalid JSON payload");
                return;
            }

            // Device
            deviceConfig.setDeviceName(doc["device"]["name"]);
            deviceConfig.setDeviceTimezone(doc["device"]["timezone"]);
            deviceConfig.setDeviceBootCommand(doc["device"]["bootCommand"]);
            deviceConfig.setDeviceSinglePress(doc["device"]["singlePress"]);
            deviceConfig.setDeviceDoublePress(doc["device"]["doublePress"]);
            deviceConfig.setDeviceLongPress(doc["device"]["longPress"]);
            deviceConfig.setDeviceUserName(doc["device"]["userName"]);
            deviceConfig.setDeviceUserPassword(doc["device"]["userPassword"]);
            
            // WiFi
            deviceConfig.setWifiSsid(doc["wifi"]["ssid"]);
            deviceConfig.setWifiPassword(doc["wifi"]["password"]);
            deviceConfig.setWifiScan(doc["wifi"]["scan"]);

            // MQTT
            deviceConfig.setMqttEnabled(doc["mqtt"]["enabled"]);
            deviceConfig.setMqttServer(doc["mqtt"]["server"]);
            deviceConfig.setMqttPort(doc["mqtt"]["port"]);
            deviceConfig.setMqttUsername(doc["mqtt"]["username"]);
            deviceConfig.setMqttPassword(doc["mqtt"]["password"]);
            deviceConfig.setMqttSsl(doc["mqtt"]["ssl"]);
            deviceConfig.setMqttSubTopic(doc["mqtt"]["subTopic"]);
            deviceConfig.setMqttPubTopic(doc["mqtt"]["pubTopic"]);
            
            // Security
            deviceConfig.setSecurityApiKey(doc["security"]["apiKey"]);
            
            WebHandler::sendSuccessResponse(request, "Settings updated successfully");

            // Optional reboot if needed
            // if (delayTimer.isReady()) {
            //     ESP.restart();
            // }
        }
    );
}

void ServeSettings::handleGetSettingsDeprecated(AsyncWebServerRequest *request)
{
    // Open the file for reading
    File file = LittleFS.open(SETTINGS_FILE, "r");
    if (!file || file.size() == 0) {
        WebHandler::sendErrorResponse(request, 400, "Failed to read settings.json or file is empty");
        return;
    }

    String json = file.readString(); // Read the file content into a string
    file.close(); // Close the file

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", json);
    WebHandler::addCorsHeaders(response);
    request->send(response);
}

void ServeSettings::handleSetSettingsDeprecated(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/deprecated/set");

    // Accumulate incoming data
    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start fresh
    }
    requestBody += String((char*)data).substring(0, len);

    // Wait until the full body is received
    if (index + len != total) {
        return;
    }

    // Parse incoming JSON
    JsonDocument newDoc;
    DeserializationError error = deserializeJson(newDoc, requestBody);
    if (error) {
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON format in request");
        return;
    }

    // Read existing settings
    JsonDocument existingDoc;
    File file = LittleFS.open(SETTINGS_FILE, "r");
    if (file) {
        error = deserializeJson(existingDoc, file);
        file.close();
        if (error) {
            WebHandler::sendErrorResponse(request, 400, "Failed to parse existing settings");
            return;
        }
    } else {
        // If file doesn't exist, we'll start with an empty document
        debugV("No existing settings file found, creating new one");
    }

    // Merge new settings into existing settings
    // Update top-level objects (device, wifi, mqtt, security)
    for (JsonPair kv : newDoc.as<JsonObject>()) {
        const char* section = kv.key().c_str();
        JsonObject newSection = kv.value().as<JsonObject>();
        JsonObject existingSection = existingDoc[section] | JsonObject();

        // Update fields within each section
        for (JsonPair field : newSection) {
            existingSection[field.key()] = field.value();
        }
    }

    // Save merged settings
    file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 400, "Failed to open settings.json for writing");
        return;
    }

    serializeJsonPretty(existingDoc, file);
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

/*
void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    // Accumulate incoming data
    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start fresh
    }
    requestBody += String((char*)data).substring(0, len);

    // Wait until the full body is received
    if (index + len != total) {
        return;
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);

    if (error) {
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON format in request");
        return;
    }

    // Save it formatted (pretty)
    File file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 400, "Failed to open settings.json for writing");
        return;
    }

    serializeJsonPretty(doc, file); // ✅ Pretty format!
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

// This should be in a separate utility file for better organization repeating the same pattern in save buttons
static void mergeJson(JsonObject dst, JsonObjectConst src) {
    for (JsonPairConst kv : src) {
      const char* key = kv.key().c_str();
  
      if (kv.value().is<JsonObject>() && dst[key].is<JsonObject>()) {
        // Recursively merge sub-objects
        mergeJson(dst[key].as<JsonObject>(), kv.value().as<JsonObjectConst>());
      } else {
        // Overwrite primitive or add new field
        dst[key] = kv.value();
      }
    }
}

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start new
    }

    requestBody += String((char*)data).substring(0, len);

    if (index + len != total) return; // Wait until full body is received

    JsonDocument incomingDoc;
    DeserializationError err = deserializeJson(incomingDoc, requestBody);
    if (err) {
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON format");
        return;
    }

    // Load existing settings if available
    JsonDocument existingDoc;
    File file = LittleFS.open(SETTINGS_FILE, "r");
    if (file) {
        deserializeJson(existingDoc, file);
        file.close();
    }

    // Merge incoming into existing
    //mergeJson(existingDoc, incomingDoc);
    mergeJson(existingDoc.as<JsonObject>(), incomingDoc.as<JsonObjectConst>());

    // Save back as pretty JSON
    file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 500, "Failed to open settings file for writing");
        return;
    }

    serializeJsonPretty(existingDoc, file);
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    // Accumulate incoming data
    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start fresh
    }
    requestBody += String((char*)data).substring(0, len);

    // Wait until the full body is received
    if (index + len != total) {
        return;
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);

    if (error) {
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON format in request");
        return;
    }

    // Save it formatted (pretty)
    File file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 400, "Failed to open settings.json for writing");
        return;
    }

    serializeJsonPretty(doc, file); // ✅ Pretty format!
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    // Open the file for writing
    File file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 400, "Failed to open settings.json for writing");
        return;
    }

    // Write the data to the file
    if (file.write(data, len) != len) {
        WebHandler::sendErrorResponse(request, 400, "Failed to write data to settings.json");
        return;
    }

    // Close the file
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

*/

#endif // ENABLE_WEB_HANDLER
