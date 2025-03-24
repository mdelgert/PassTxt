#pragma once

#ifdef ENABLE_WIFI_HANDLER

#include "Globals.h"
#include "GfxHandler.h"
#include "RemoteDebugHandler.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class WifiHandler
{
private:
    static void connectToWifi();
    static void startAccessPoint();
    static void initializeMDNS();
    static void scanAndSaveNetworks(const char *filePath);
public:
    static void init();
    static void loop();
};

#else  // If ENABLE_WIFI_HANDLER is NOT defined, use a no-op class

class WifiHandler
{
public:
    static void init() {}  // No-op
    static void loop() {}  // No-op
};

#endif // ENABLE_WIFI_HANDLER