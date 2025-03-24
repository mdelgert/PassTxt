#pragma once

#include "Globals.h"
#include <Preferences.h>
#include <Arduino.h>

// Sub-struct for device info
struct DeviceSettings {
    String name;
    bool setupMode;
    String timezone;
    int defaultTimeout;
    uint64_t bootCount;
    uint64_t bootTime;
    uint64_t upTime;

    // Device security
    String userName;
    String userPassword;

    // Button commands
    String singlePress;
    String doublePress;
    String longPress;

    // Boot command
    String bootCommand;
};

// Sub-struct for Wi-Fi
struct WiFiSettings {
    String ssid;
    String password;
    bool scan;
};

// Sub-struct for MQTT
struct MqttSettings {
    bool enabled;
    String server;
    int port;
    bool ssl;
    String username; //todo fix name to userName for consistency
    String password; //todo fix name to userPassword for consistency
    String subTopic;
    String pubTopic;
};

// Sub-struct for security/auth
struct SecuritySettings {
    String apiKey;
    bool apiToken;
    String otaPassword;
};

// Sub-struct for feature toggles
struct FeatureSettings {
    bool cors;
    bool webHandler;
};

// Main Settings container
struct Settings {
    DeviceSettings device;
    WiFiSettings wifi;
    MqttSettings mqtt;
    SecuritySettings security;
    FeatureSettings features;
};

// Global settings object
extern Settings settings;

// ConfigManager class declaration
class ConfigManager {
public:
    static void init();
    static void save();
    static void clear();
    static void savePreferences();
    static void clearPreferences();

private:
    static void load();
    static void loadPreferences();
    static Preferences preferences;
};