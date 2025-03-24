#include "ConfigManager.h"

// Global settings object
Settings settings;

// Preferences instance
Preferences ConfigManager::preferences;

// Namespace for preferences
const char *ns = "config";

// Initialize preferences and load settings
void ConfigManager::init() {
    load();
}

// Load settings with defaults
void ConfigManager::load() {
    if (!preferences.begin(ns, false)) {
        debugE("Failed to open preferences for loading.");
        return;
    }

    settings.wifiSSID = preferences.getString("wifiSSID", settings.wifiSSID);
    settings.wifiPassword = preferences.getString("wifiPassword", settings.wifiPassword);
    settings.deviceName = preferences.getString("deviceName", settings.deviceName);
    settings.setupMode = preferences.getBool("setupMode", settings.setupMode);
    settings.timeout = preferences.getInt("timeout", settings.timeout);
    settings.ledEnabled = preferences.getBool("ledEnabled", settings.ledEnabled);

    preferences.end();
}

// Save settings to preferences
void ConfigManager::save() {
    if (!preferences.begin(ns, false)) {
        debugE("Failed to open preferences for saving.");
        return;
    }

    preferences.putString("wifiSSID", settings.wifiSSID);
    preferences.putString("wifiPassword", settings.wifiPassword);
    preferences.putString("deviceName", settings.deviceName);
    preferences.putBool("setupMode", settings.setupMode);
    preferences.putInt("timeout", settings.timeout);
    preferences.putBool("ledEnabled", settings.ledEnabled);

    preferences.end();
}

// Clear all preferences
void ConfigManager::clear() {
    if (!preferences.begin(ns, false)) {
        debugE("Failed to open preferences for clearing.");
        return;
    }

    preferences.clear();
    preferences.end();

    // Reset settings to defaults
    settings = Settings();
}
