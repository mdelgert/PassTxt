#include "PreferencesHandler.h"

// Define static members
Preferences PreferencesHandler::preferences;
const char* PreferencesHandler::namespaceName = nullptr;
bool PreferencesHandler::initialized = false;

// Global configuration variables
String PreferencesHandler::deviceName = SECURE_DEVICE_NAME;
String PreferencesHandler::wifiSSID = SECURE_WIFI_SSID;
String PreferencesHandler::wifiPassword = SECURE_WIFI_PASSWORD;
String PreferencesHandler::mqttServer = SECURE_MQTT_SERVER;
String PreferencesHandler::mqttPort = SECURE_MQTT_PORT;
String PreferencesHandler::mqttUsername = SECURE_MQTT_USERNAME;
String PreferencesHandler::mqttPassword = SECURE_MQTT_PASSWORD;
String PreferencesHandler::apiKey = SECURE_API_KEY;

void PreferencesHandler::ensureInitialized() {
    if (!initialized) {
        preferences.begin(namespaceName, false);
        initialized = true;
        debugI("PreferencesHandler initialized with namespace: %s", namespaceName);
    }
}

void PreferencesHandler::init(const char* namespaceName) {
    PreferencesHandler::namespaceName = namespaceName;
    ensureInitialized();
    loadGlobalConfig(); // Load all configuration values into memory
    debugI("PreferencesHandler initialized and global configuration loaded.");
}

void PreferencesHandler::clear() {
    ensureInitialized();
    preferences.clear();
    debugI("Preferences cleared for namespace: %s", namespaceName);
}

void PreferencesHandler::setValue(const String& key, const String& value) {
    ensureInitialized();
    preferences.putString(key.c_str(), value);
    debugI("Set key: %s to value: %s", key.c_str(), value.c_str());
}

bool PreferencesHandler::getValue(const String& key, String& outValue, const String& fallback) {
    ensureInitialized();
    outValue = preferences.getString(key.c_str(), "");
    if (outValue.isEmpty()) {
        debugW("Key: %s not found or has an empty value. Using fallback: %s", key.c_str(), fallback.c_str());
        outValue = fallback;
        return false; // Indicates fallback was used
    }
    debugI("Retrieved key: %s with value: %s", key.c_str(), outValue.c_str());
    return true; // Indicates value was found
}

void PreferencesHandler::loadGlobalConfig() {
    debugI("Loading global configuration...");

    // Load preferences with fallbacks from SecureExample.h
    getValue("device_name", deviceName, SECURE_DEVICE_NAME);
    getValue("wifi_ssid", wifiSSID, SECURE_WIFI_SSID);
    getValue("wifi_password", wifiPassword, SECURE_WIFI_PASSWORD);
    getValue("mqtt_server", mqttServer, SECURE_MQTT_SERVER);
    getValue("mqtt_port", mqttPort, SECURE_MQTT_PORT);
    getValue("mqtt_username", mqttUsername, SECURE_MQTT_USERNAME);
    getValue("mqtt_password", mqttPassword, SECURE_MQTT_PASSWORD);
    getValue("api_key", apiKey, SECURE_API_KEY);

    debugI("Global configuration loaded successfully.");
}

// Accessors for global configuration variables
const String& PreferencesHandler::getDeviceName() { return deviceName; }
const String& PreferencesHandler::getWiFiSSID() { return wifiSSID; }
const String& PreferencesHandler::getWiFiPassword() { return wifiPassword; }
const String& PreferencesHandler::getMQTTServer() { return mqttServer; }
const String& PreferencesHandler::getMQTTPort() { return mqttPort; }
const String& PreferencesHandler::getMQTTUsername() { return mqttUsername; }
const String& PreferencesHandler::getMQTTPassword() { return mqttPassword; }
const String& PreferencesHandler::getAPIKey() { return apiKey; }