// DeviceConfig.h
#pragma once

#include <Preferences.h>
#include <Arduino.h>

class DeviceConfig {
private:
    static DeviceConfig instance; // Single static instance
    Preferences prefs;            // For NVS storage

    // Device settings
    char deviceName[32];
    bool deviceSetupMode;
    char deviceTimezone[32];
    int deviceDefaultTimeout;
    int deviceKeyPressDelay;
    uint64_t deviceBootCount;
    uint64_t deviceBootTime;
    uint64_t deviceUpTime;
    char deviceUserName[32];
    char deviceUserPassword[32];
    char deviceSinglePress[32];
    char deviceDoublePress[32];
    char deviceLongPress[32];
    char deviceBootCommand[32];

    // Wi-Fi settings
    char wifiSsid[32];
    char wifiPassword[64];
    bool wifiScan;

    // MQTT settings
    bool mqttEnabled;
    char mqttServer[64];
    int mqttPort;
    bool mqttSsl;
    char mqttUsername[32];
    char mqttPassword[32];
    char mqttSubTopic[64];
    char mqttPubTopic[64];
    bool mqttIsConnected;

    // Security settings
    char securityApiKey[64];
    bool securityApiToken;
    char securityOtaPassword[32];
    
    // Feature settings
    bool featuresCors;
    bool featuresWebHandler;

    DeviceConfig();           // Private constructor
    void loadFromNVS();       // Load settings from NVS
    void saveToNVS();         // Save settings to NVS

public:
    static DeviceConfig& getInstance(); // Get the single instance

    void begin();                      // Initialize settings
    void clear();                      // Clear all settings

    // Device settings getters and setters
    const char* getDeviceName() const;
    void setDeviceName(const char* name);
    bool getDeviceSetupMode() const;
    void setDeviceSetupMode(bool mode);
    const char* getDeviceTimezone() const;
    void setDeviceTimezone(const char* tz);
    int getDeviceDefaultTimeout() const;
    void setDeviceDefaultTimeout(int timeout);
    int getDeviceKeyPressDelay() const;
    void setDeviceKeyPressDelay(int delay);
    uint64_t getDeviceBootCount() const;
    void setDeviceBootCount(uint64_t count);
    uint64_t getDeviceBootTime() const;
    void setDeviceBootTime(uint64_t time);
    uint64_t getDeviceUpTime() const;
    void setDeviceUpTime(uint64_t time);
    const char* getDeviceUserName() const;
    void setDeviceUserName(const char* name);
    const char* getDeviceUserPassword() const;
    void setDeviceUserPassword(const char* pass);
    const char* getDeviceSinglePress() const;
    void setDeviceSinglePress(const char* cmd);
    const char* getDeviceDoublePress() const;
    void setDeviceDoublePress(const char* cmd);
    const char* getDeviceLongPress() const;
    void setDeviceLongPress(const char* cmd);
    const char* getDeviceBootCommand() const;
    void setDeviceBootCommand(const char* cmd);

    // Wi-Fi settings getters and setters
    const char* getWifiSsid() const;
    void setWifiSsid(const char* ssid);
    const char* getWifiPassword() const;
    void setWifiPassword(const char* pass);
    bool getWifiScan() const;
    void setWifiScan(bool scan);

    // MQTT settings getters and setters
    bool getMqttEnabled() const;
    void setMqttEnabled(bool enabled);
    const char* getMqttServer() const;
    void setMqttServer(const char* server);
    int getMqttPort() const;
    void setMqttPort(int port);
    bool getMqttSsl() const;
    void setMqttSsl(bool ssl);
    const char* getMqttUsername() const;
    void setMqttUsername(const char* name);
    const char* getMqttPassword() const;
    void setMqttPassword(const char* pass);
    const char* getMqttSubTopic() const;
    void setMqttSubTopic(const char* topic);
    const char* getMqttPubTopic() const;
    void setMqttPubTopic(const char* topic);
    bool getMqttIsConnected() const;
    void setMqttIsConnected(bool connected);

    // Security settings getters and setters
    const char* getSecurityApiKey() const;
    void setSecurityApiKey(const char* key);
    bool getSecurityApiToken() const;
    void setSecurityApiToken(bool token);
    const char* getSecurityOtaPassword() const;
    void setSecurityOtaPassword(const char* pass);

    // Feature settings getters and setters
    bool getFeaturesCors() const;
    void setFeaturesCors(bool cors);
    bool getFeaturesWebHandler() const;
    void setFeaturesWebHandler(bool web);
};

// Global reference to the Singleton instance
extern DeviceConfig& deviceConfig;