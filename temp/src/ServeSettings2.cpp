#include "ServeSettings.h"

static NonBlockingTimer delayTimer(500);

void ServeSettings::registerEndpoints(AsyncWebServer &server)
{
    handleGetSettings(server);
    handleSetSettings(server);
}

void ServeSettings::handleGetSettings(AsyncWebServer &server)
{
    server.on("/settings/get", HTTP_GET, [](AsyncWebServerRequest *request) {
        debugI("Received GET request on /settings/get");

        // Prepare JSON response with global configuration values
        JsonDocument doc;
        doc["device_name"] = PreferencesHandler::getDeviceName();
        doc["wifi_ssid"] = PreferencesHandler::getWiFiSSID();
        doc["wifi_password"] = PreferencesHandler::getWiFiPassword();
        doc["mqtt_server"] = PreferencesHandler::getMQTTServer();
        doc["mqtt_port"] = PreferencesHandler::getMQTTPort();
        doc["mqtt_username"] = PreferencesHandler::getMQTTUsername();
        doc["mqtt_password"] = PreferencesHandler::getMQTTPassword();
        doc["api_key"] = PreferencesHandler::getAPIKey();

        WebHandler::sendSuccessResponse(request, "Settings retrieved successfully", &doc);
    });
}

void ServeSettings::handleSetSettings(AsyncWebServer &server)
{
    server.on("/settings/set", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        debugI("Received POST request on /settings/set");

        // Print the request body for debugging
        WebHandler::printRequestBody(request, data, len);

        // Parse JSON payload
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
            debugE("JSON deserialization failed: %s", error.c_str());
            WebHandler::sendErrorResponse(request, 400, "Invalid JSON payload");
            return;
        }

        // Update settings in memory using JSON data or keep existing values
        PreferencesHandler::setValue("device_name", doc["device_name"] | PreferencesHandler::getDeviceName());
        PreferencesHandler::setValue("wifi_ssid", doc["wifi_ssid"] | PreferencesHandler::getWiFiSSID());
        PreferencesHandler::setValue("wifi_password", doc["wifi_password"] | PreferencesHandler::getWiFiPassword());
        PreferencesHandler::setValue("mqtt_server", doc["mqtt_server"] | PreferencesHandler::getMQTTServer());
        PreferencesHandler::setValue("mqtt_port", doc["mqtt_port"] | PreferencesHandler::getMQTTPort());
        PreferencesHandler::setValue("mqtt_username", doc["mqtt_username"] | PreferencesHandler::getMQTTUsername());
        PreferencesHandler::setValue("mqtt_password", doc["mqtt_password"] | PreferencesHandler::getMQTTPassword());
        PreferencesHandler::setValue("api_key", doc["api_key"] | PreferencesHandler::getAPIKey());

        // Reload global settings to reflect updates
        PreferencesHandler::loadGlobalConfig();

        // Validate mandatory fields (example: Wi-Fi credentials)
        if (PreferencesHandler::getWiFiSSID().isEmpty() || PreferencesHandler::getWiFiPassword().isEmpty()) {
            debugE("Wi-Fi credentials missing");
            WebHandler::sendErrorResponse(request, 400, "Missing Wi-Fi credentials");
            return;
        }

        // Send success response
        WebHandler::sendSuccessResponse(request, "Settings updated successfully");

        if (delayTimer.isReady())
        {
            ESP.restart();
        }
    });
}
