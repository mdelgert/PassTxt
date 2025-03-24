#include "ServeSettings.h"

// Static variables to store settings
static String device_name = SECURE_DEVICE_NAME;
static String wifi_ssid = SECURE_WIFI_SSID;
static String wifi_password = SECURE_WIFI_PASSWORD;
static String mqtt_server = SECURE_MQTT_SERVER;
static String mqtt_port = SECURE_MQTT_PORT;
static String mqtt_username = SECURE_MQTT_USERNAME;
static String mqtt_password = SECURE_MQTT_PASSWORD;
static String api_key = SECURE_API_KEY;

// Endpoint constants
static const char* GET_SETTINGS_ENDPOINT = "/settings/get";
static const char* SET_SETTINGS_ENDPOINT = "/settings/set";

void ServeSettings::registerEndpoints(AsyncWebServer &server)
{
    // Register all endpoints
    server.on(GET_SETTINGS_ENDPOINT, HTTP_GET, handleGetSettings);
    server.on(SET_SETTINGS_ENDPOINT, HTTP_POST, nullptr, nullptr, handleSetSettings);
}

void ServeSettings::handleGetSettings(AsyncWebServerRequest *request)
{
    debugI("Received GET request on %s", GET_SETTINGS_ENDPOINT);

    // Fetch settings from preferences with fallback to defaults
    PreferencesHandler::getValue("device_name", device_name, SECURE_DEVICE_NAME);
    PreferencesHandler::getValue("wifi_ssid", wifi_ssid, SECURE_WIFI_SSID);
    PreferencesHandler::getValue("wifi_password", wifi_password, SECURE_WIFI_PASSWORD);
    PreferencesHandler::getValue("mqtt_server", mqtt_server, SECURE_MQTT_SERVER);
    PreferencesHandler::getValue("mqtt_port", mqtt_port, SECURE_MQTT_PORT);
    PreferencesHandler::getValue("mqtt_username", mqtt_username, SECURE_MQTT_USERNAME);
    PreferencesHandler::getValue("mqtt_password", mqtt_password, SECURE_MQTT_PASSWORD);
    PreferencesHandler::getValue("api_key", api_key, SECURE_API_KEY);

    // Prepare JSON response
    JsonDocument data;
    data["device_name"] = device_name;
    data["wifi_ssid"] = wifi_ssid;
    data["wifi_password"] = wifi_password;
    data["mqtt_server"] = mqtt_server;
    data["mqtt_port"] = mqtt_port;
    data["mqtt_username"] = mqtt_username;
    data["mqtt_password"] = mqtt_password;
    data["api_key"] = api_key;

    // Send response
    WebHandler::sendSuccessResponse(request, "Settings retrieved successfully", &data);
}

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugI("Received POST request on %s", SET_SETTINGS_ENDPOINT);

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

    // Update settings from JSON
    device_name = doc["device_name"] | device_name;
    wifi_ssid = doc["wifi_ssid"] | wifi_ssid;
    wifi_password = doc["wifi_password"] | wifi_password;
    mqtt_server = doc["mqtt_server"] | mqtt_server;
    mqtt_port = doc["mqtt_port"] | mqtt_port;
    mqtt_username = doc["mqtt_username"] | mqtt_username;
    mqtt_password = doc["mqtt_password"] | mqtt_password;
    api_key = doc["api_key"] | api_key;

    // Validate mandatory fields (example for Wi-Fi credentials)
    if (wifi_ssid.isEmpty() || wifi_password.isEmpty()) {
        debugE("Wi-Fi credentials missing");
        WebHandler::sendErrorResponse(request, 400, "Missing Wi-Fi credentials");
        return;
    }

    // Save updated settings to preferences
    PreferencesHandler::setValue("device_name", device_name);
    PreferencesHandler::setValue("wifi_ssid", wifi_ssid);
    PreferencesHandler::setValue("wifi_password", wifi_password);
    PreferencesHandler::setValue("mqtt_server", mqtt_server);
    PreferencesHandler::setValue("mqtt_port", mqtt_port);
    PreferencesHandler::setValue("mqtt_username", mqtt_username);
    PreferencesHandler::setValue("mqtt_password", mqtt_password);
    PreferencesHandler::setValue("api_key", api_key);

    // Send success response
    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}
