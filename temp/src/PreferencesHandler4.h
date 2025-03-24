#ifndef PREFERENCES_HANDLER_H
#define PREFERENCES_HANDLER_H

#include "Globals.h"
#include <Preferences.h>
#include <Arduino.h>

class PreferencesHandler {
public:
    static void init(const char* namespaceName);
    static void clear();
    static void setValue(const String& key, const String& value);
    static bool getValue(const String& key, String& outValue, const String& fallback);
    static void loadGlobalConfig();

    // Access global configuration values
    static const String& getDeviceName();
    static const String& getWiFiSSID();
    static const String& getWiFiPassword();
    static const String& getMQTTServer();
    static const String& getMQTTPort();
    static const String& getMQTTUsername();
    static const String& getMQTTPassword();
    static const String& getAPIKey();

private:
    static void ensureInitialized();

    static Preferences preferences;
    static const char* namespaceName;
    static bool initialized;

    // Global configuration variables
    static String deviceName;
    static String wifiSSID;
    static String wifiPassword;
    static String mqttServer;
    static String mqttPort;
    static String mqttUsername;
    static String mqttPassword;
    static String apiKey;
};

#endif // PREFERENCES_HANDLER_H
