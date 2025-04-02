// DeviceConfig.cpp
#include "DeviceConfig.h"
#include "Globals.h"

// Define the static instance
DeviceConfig DeviceConfig::instance;

// Define the global reference
DeviceConfig& deviceConfig = DeviceConfig::getInstance();

// Private constructor with defaults
DeviceConfig::DeviceConfig() {
    strcpy(deviceName, "passtxt5");
    deviceSetupMode = false;
    strcpy(deviceTimezone, "America/New_York");
    deviceDefaultTimeout = 30;
    deviceKeyPressDelay = 50;
    deviceBootCount = 0;
    deviceBootTime = 0;
    deviceUpTime = 0;
    strcpy(deviceUserName, "admin");
    strcpy(deviceUserPassword, "pasword");
    strcpy(deviceSinglePress, "hid file /data/mqtt1.crt");
    strcpy(deviceDoublePress, "");
    strcpy(deviceLongPress, "");
    strcpy(deviceBootCommand, "");

    strcpy(wifiSsid, SECURE_WIFI_SSID);
    strcpy(wifiPassword, SECURE_WIFI_PASSWORD);
    wifiScan = true;

    mqttEnabled = true; // Enable MQTT by default
    strcpy(mqttServer, SECURE_MQTT_SERVER);
    mqttPort = SECURE_MQTT_PORT;
    mqttSsl = SECURE_MQTT_SSL;
    strcpy(mqttUsername, SECURE_MQTT_USERNAME);
    strcpy(mqttPassword, SECURE_MQTT_PASSWORD);
    strcpy(mqttSubTopic, "passtxt/in");
    strcpy(mqttPubTopic, "passtxt/out");
    mqttIsConnected = false;

    strcpy(securityApiKey, "abcdef123456");
    securityApiToken = false;
    strcpy(securityOtaPassword, "password");

    featuresCors = false;
    featuresWebHandler = true;
}

// Get the single instance
DeviceConfig& DeviceConfig::getInstance() {
    return instance;
}

// Initialize settings
void DeviceConfig::begin() {
    loadFromNVS();
}

// Clear all settings (reset to defaults)
void DeviceConfig::clear() {
    prefs.begin("app_settings", false);
    prefs.clear();
    prefs.end();
    // Reset to constructor defaults
    DeviceConfig temp;
    *this = temp;
    saveToNVS();
}

// Load all settings from NVS
void DeviceConfig::loadFromNVS() {
    prefs.begin("app_settings", true); // Read-only mode
    prefs.getString("device_name", deviceName, sizeof(deviceName));
    deviceSetupMode = prefs.getBool("device_setup_mode", deviceSetupMode);
    prefs.getString("device_timezone", deviceTimezone, sizeof(deviceTimezone));
    deviceDefaultTimeout = prefs.getInt("device_timeout", deviceDefaultTimeout);
    deviceKeyPressDelay = prefs.getInt("device_key_delay", deviceKeyPressDelay);
    deviceBootCount = prefs.getULong64("device_boot_count", deviceBootCount);
    deviceBootTime = prefs.getULong64("device_boot_time", deviceBootTime);
    deviceUpTime = prefs.getULong64("device_up_time", deviceUpTime);
    prefs.getString("device_username", deviceUserName, sizeof(deviceUserName));
    prefs.getString("device_userpass", deviceUserPassword, sizeof(deviceUserPassword));
    prefs.getString("device_single_press", deviceSinglePress, sizeof(deviceSinglePress));
    prefs.getString("device_double_press", deviceDoublePress, sizeof(deviceDoublePress));
    prefs.getString("device_long_press", deviceLongPress, sizeof(deviceLongPress));
    prefs.getString("device_boot_cmd", deviceBootCommand, sizeof(deviceBootCommand));

    prefs.getString("wifi_ssid", wifiSsid, sizeof(wifiSsid));
    prefs.getString("wifi_pass", wifiPassword, sizeof(wifiPassword));
    wifiScan = prefs.getBool("wifi_scan", wifiScan);

    mqttEnabled = prefs.getBool("mqtt_enabled", mqttEnabled);
    prefs.getString("mqtt_server", mqttServer, sizeof(mqttServer));
    mqttPort = prefs.getInt("mqtt_port", mqttPort);
    mqttSsl = prefs.getBool("mqtt_ssl", mqttSsl);
    prefs.getString("mqtt_username", mqttUsername, sizeof(mqttUsername));
    prefs.getString("mqtt_pass", mqttPassword, sizeof(mqttPassword));
    prefs.getString("mqtt_sub_topic", mqttSubTopic, sizeof(mqttSubTopic));
    prefs.getString("mqtt_pub_topic", mqttPubTopic, sizeof(mqttPubTopic));
    mqttIsConnected = prefs.getBool("mqtt_connected", mqttIsConnected);

    prefs.getString("security_api_key", securityApiKey, sizeof(securityApiKey));
    securityApiToken = prefs.getBool("security_api_token", securityApiToken);
    prefs.getString("security_ota_pass", securityOtaPassword, sizeof(securityOtaPassword));

    featuresCors = prefs.getBool("features_cors", featuresCors);
    featuresWebHandler = prefs.getBool("features_web", featuresWebHandler);
    prefs.end();
}

// Save all settings to NVS
void DeviceConfig::saveToNVS() {
    prefs.begin("app_settings", false); // Read-write mode
    prefs.putString("device_name", deviceName);
    prefs.putBool("device_setup_mode", deviceSetupMode);
    prefs.putString("device_timezone", deviceTimezone);
    prefs.putInt("device_timeout", deviceDefaultTimeout);
    prefs.putInt("device_key_delay", deviceKeyPressDelay);
    prefs.putULong64("device_boot_count", deviceBootCount);
    prefs.putULong64("device_boot_time", deviceBootTime);
    prefs.putULong64("device_up_time", deviceUpTime);
    prefs.putString("device_username", deviceUserName);
    prefs.putString("device_userpass", deviceUserPassword);
    prefs.putString("device_single_press", deviceSinglePress);
    prefs.putString("device_double_press", deviceDoublePress);
    prefs.putString("device_long_press", deviceLongPress);
    prefs.putString("device_boot_cmd", deviceBootCommand);

    prefs.putString("wifi_ssid", wifiSsid);
    prefs.putString("wifi_pass", wifiPassword);
    prefs.putBool("wifi_scan", wifiScan);

    prefs.putBool("mqtt_enabled", mqttEnabled);
    prefs.putString("mqtt_server", mqttServer);
    prefs.putInt("mqtt_port", mqttPort);
    prefs.putBool("mqtt_ssl", mqttSsl);
    prefs.putString("mqtt_username", mqttUsername);
    prefs.putString("mqtt_pass", mqttPassword);
    prefs.putString("mqtt_sub_topic", mqttSubTopic);
    prefs.putString("mqtt_pub_topic", mqttPubTopic);
    prefs.putBool("mqtt_connected", mqttIsConnected);

    prefs.putString("security_api_key", securityApiKey);
    prefs.putBool("security_api_token", securityApiToken);
    prefs.putString("security_ota_pass", securityOtaPassword);

    prefs.putBool("features_cors", featuresCors);
    prefs.putBool("features_web", featuresWebHandler);
    prefs.end();
}

// Device settings getters and setters
const char* DeviceConfig::getDeviceName() const { return deviceName; }
void DeviceConfig::setDeviceName(const char* name) { strncpy(deviceName, name, sizeof(deviceName) - 1); deviceName[sizeof(deviceName) - 1] = '\0'; saveToNVS(); }
bool DeviceConfig::getDeviceSetupMode() const { return deviceSetupMode; }
void DeviceConfig::setDeviceSetupMode(bool mode) { deviceSetupMode = mode; saveToNVS(); }
const char* DeviceConfig::getDeviceTimezone() const { return deviceTimezone; }
void DeviceConfig::setDeviceTimezone(const char* tz) { strncpy(deviceTimezone, tz, sizeof(deviceTimezone) - 1); deviceTimezone[sizeof(deviceTimezone) - 1] = '\0'; saveToNVS(); }
int DeviceConfig::getDeviceDefaultTimeout() const { return deviceDefaultTimeout; }
void DeviceConfig::setDeviceDefaultTimeout(int timeout) { deviceDefaultTimeout = timeout; saveToNVS(); }
int DeviceConfig::getDeviceKeyPressDelay() const { return deviceKeyPressDelay; }
void DeviceConfig::setDeviceKeyPressDelay(int delay) { deviceKeyPressDelay = delay; saveToNVS(); }
uint64_t DeviceConfig::getDeviceBootCount() const { return deviceBootCount; }
void DeviceConfig::setDeviceBootCount(uint64_t count) { deviceBootCount = count; saveToNVS(); }
uint64_t DeviceConfig::getDeviceBootTime() const { return deviceBootTime; }
void DeviceConfig::setDeviceBootTime(uint64_t time) { deviceBootTime = time; saveToNVS(); }

uint64_t DeviceConfig::getDeviceUpTime() const { return deviceUpTime; }
void DeviceConfig::setDeviceUpTime(uint64_t time) 
{ 
  deviceUpTime = time;
  //Reduce writes to NVS to avoid wear and tear
  //saveToNVS(); 
}

const char* DeviceConfig::getDeviceUserName() const { return deviceUserName; }
void DeviceConfig::setDeviceUserName(const char* name) { strncpy(deviceUserName, name, sizeof(deviceUserName) - 1); deviceUserName[sizeof(deviceUserName) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getDeviceUserPassword() const { return deviceUserPassword; }
void DeviceConfig::setDeviceUserPassword(const char* pass) { strncpy(deviceUserPassword, pass, sizeof(deviceUserPassword) - 1); deviceUserPassword[sizeof(deviceUserPassword) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getDeviceSinglePress() const { return deviceSinglePress; }
void DeviceConfig::setDeviceSinglePress(const char* cmd) { strncpy(deviceSinglePress, cmd, sizeof(deviceSinglePress) - 1); deviceSinglePress[sizeof(deviceSinglePress) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getDeviceDoublePress() const { return deviceDoublePress; }
void DeviceConfig::setDeviceDoublePress(const char* cmd) { strncpy(deviceDoublePress, cmd, sizeof(deviceDoublePress) - 1); deviceDoublePress[sizeof(deviceDoublePress) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getDeviceLongPress() const { return deviceLongPress; }
void DeviceConfig::setDeviceLongPress(const char* cmd) { strncpy(deviceLongPress, cmd, sizeof(deviceLongPress) - 1); deviceLongPress[sizeof(deviceLongPress) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getDeviceBootCommand() const { return deviceBootCommand; }
void DeviceConfig::setDeviceBootCommand(const char* cmd) { strncpy(deviceBootCommand, cmd, sizeof(deviceBootCommand) - 1); deviceBootCommand[sizeof(deviceBootCommand) - 1] = '\0'; saveToNVS(); }

// Wi-Fi settings getters and setters
const char* DeviceConfig::getWifiSsid() const { return wifiSsid; }
void DeviceConfig::setWifiSsid(const char* ssid) { strncpy(wifiSsid, ssid, sizeof(wifiSsid) - 1); wifiSsid[sizeof(wifiSsid) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getWifiPassword() const { return wifiPassword; }
void DeviceConfig::setWifiPassword(const char* pass) { strncpy(wifiPassword, pass, sizeof(wifiPassword) - 1); wifiPassword[sizeof(wifiPassword) - 1] = '\0'; saveToNVS(); }
bool DeviceConfig::getWifiScan() const { return wifiScan; }
void DeviceConfig::setWifiScan(bool scan) { wifiScan = scan; saveToNVS(); }

// MQTT settings getters and setters
bool DeviceConfig::getMqttEnabled() const { return mqttEnabled; }
void DeviceConfig::setMqttEnabled(bool enabled) { mqttEnabled = enabled; saveToNVS(); }
const char* DeviceConfig::getMqttServer() const { return mqttServer; }
void DeviceConfig::setMqttServer(const char* server) { strncpy(mqttServer, server, sizeof(mqttServer) - 1); mqttServer[sizeof(mqttServer) - 1] = '\0'; saveToNVS(); }
int DeviceConfig::getMqttPort() const { return mqttPort; }
void DeviceConfig::setMqttPort(int port) { mqttPort = port; saveToNVS(); }
bool DeviceConfig::getMqttSsl() const { return mqttSsl; }
void DeviceConfig::setMqttSsl(bool ssl) { mqttSsl = ssl; saveToNVS(); }
const char* DeviceConfig::getMqttUsername() const { return mqttUsername; }
void DeviceConfig::setMqttUsername(const char* name) { strncpy(mqttUsername, name, sizeof(mqttUsername) - 1); mqttUsername[sizeof(mqttUsername) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getMqttPassword() const { return mqttPassword; }
void DeviceConfig::setMqttPassword(const char* pass) { strncpy(mqttPassword, pass, sizeof(mqttPassword) - 1); mqttPassword[sizeof(mqttPassword) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getMqttSubTopic() const { return mqttSubTopic; }
void DeviceConfig::setMqttSubTopic(const char* topic) { strncpy(mqttSubTopic, topic, sizeof(mqttSubTopic) - 1); mqttSubTopic[sizeof(mqttSubTopic) - 1] = '\0'; saveToNVS(); }
const char* DeviceConfig::getMqttPubTopic() const { return mqttPubTopic; }
void DeviceConfig::setMqttPubTopic(const char* topic) { strncpy(mqttPubTopic, topic, sizeof(mqttPubTopic) - 1); mqttPubTopic[sizeof(mqttPubTopic) - 1] = '\0'; saveToNVS(); }

bool DeviceConfig::getMqttIsConnected() const { return mqttIsConnected; }
void DeviceConfig::setMqttIsConnected(bool connected) 
{ 
    mqttIsConnected = connected;
    // Reduce the frequency of saving to NVS to avoid excessive writes
    //saveToNVS(); 
}

// Security settings getters and setters
const char* DeviceConfig::getSecurityApiKey() const { return securityApiKey; }
void DeviceConfig::setSecurityApiKey(const char* key) { strncpy(securityApiKey, key, sizeof(securityApiKey) - 1); securityApiKey[sizeof(securityApiKey) - 1] = '\0'; saveToNVS(); }
bool DeviceConfig::getSecurityApiToken() const { return securityApiToken; }
void DeviceConfig::setSecurityApiToken(bool token) { securityApiToken = token; saveToNVS(); }
const char* DeviceConfig::getSecurityOtaPassword() const { return securityOtaPassword; }
void DeviceConfig::setSecurityOtaPassword(const char* pass) { strncpy(securityOtaPassword, pass, sizeof(securityOtaPassword) - 1); securityOtaPassword[sizeof(securityOtaPassword) - 1] = '\0'; saveToNVS(); }

// Feature settings getters and setters
bool DeviceConfig::getFeaturesCors() const { return featuresCors; }
void DeviceConfig::setFeaturesCors(bool cors) { featuresCors = cors; saveToNVS(); }
bool DeviceConfig::getFeaturesWebHandler() const { return featuresWebHandler; }
void DeviceConfig::setFeaturesWebHandler(bool web) { featuresWebHandler = web; saveToNVS(); }