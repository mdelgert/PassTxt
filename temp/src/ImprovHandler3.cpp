#ifdef ENABLE_IMPROV_HANDLER

#include "ImprovHandler.h"

ImprovWiFi ImprovHandler::improvSerial(&Serial);

void ImprovHandler::init()
{
    //Todo cleanup wifi handler as if in AP mode ImprovWiFi will not work
    //WiFi.mode(WIFI_STA);
    improvSerial.setDeviceInfo(ImprovTypes::
                                   ChipFamily::CF_ESP32_S3,
                               "ImprovWiFi", SOFTWARE_VERSION,
                               "Demo", "http://{LOCAL_IPV4}");

    improvSerial.onImprovError(onImprovWiFiErrorCb);
    improvSerial.onImprovConnected(onImprovWiFiConnectedCb);
    debugD("ImprovWiFi initialized");
}

void ImprovHandler::loop()
{
    improvSerial.handleSerial();
}

void ImprovHandler::onImprovWiFiErrorCb(ImprovTypes::Error err)
{
    debugE("ImprovWiFi error occurred: %d", err);
}

void ImprovHandler::onImprovWiFiConnectedCb(const char *ssid, const char *password)
{
    settings.wifiSSID = String(ssid);
    settings.wifiPassword = String(password);
    ConfigManager::save();
    debugD("WiFi credentials saved to preferences. SSID: %s", ssid);
    GfxHandler::printMessage("Rebooting...");
    debugI("Rebooting...");
    delay(1000);
    ESP.restart();
}

#endif // ENABLE_IMPROV_HANDLER