#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "Globals.h"
#include <Preferences.h>
#include <Arduino.h>

// Struct for all settings
struct Settings {
    String wifiSSID = "defaultSSID";
    String wifiPassword = "defaultPassword";
    String deviceName = "ESP32_Device";
    bool setupMode = true;
    int timeout = 30;
    bool ledEnabled = false;
};

// Global settings object
extern Settings settings;

// ConfigManager class declaration
class ConfigManager {
public:
    static void init();
    static void save();
    static void clear();

private:
    static void load();
    static Preferences preferences;
};

#endif // CONFIGMANAGER_H

/* Example usage:

#include <Arduino.h>
#include "ConfigManager.h"

void setup() {
    Serial.begin(115200);

    // Initialize configuration
    ConfigManager::init();

    // Access settings directly without declaring them
    Serial.println("Initial Settings:");
    Serial.println("WiFi SSID: " + settings.wifiSSID);
    Serial.println("Device Name: " + settings.deviceName);
    Serial.println("Setup Mode: " + String(settings.setupMode));
    Serial.println("Timeout: " + String(settings.timeout));
    Serial.println("LED Enabled: " + String(settings.ledEnabled));

    // Modify settings directly
    settings.wifiSSID = "MyNewSSID";
    settings.wifiPassword = "NewPassword";
    settings.deviceName = "NewDevice";
    settings.setupMode = false;
    settings.timeout = 60;
    settings.ledEnabled = true;

    // Save updated settings
    ConfigManager::save();

    Serial.println("\nUpdated Settings Saved!");
}

void loop() {
    // Example: use settings
    if (settings.ledEnabled) {
        Serial.println("LED is enabled. Performing LED actions...");
        delay(1000);
    }
}
*/