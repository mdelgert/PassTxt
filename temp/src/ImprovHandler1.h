// ImprovHandler.h
#ifndef IMPROV_HANDLER_H
#define IMPROV_HANDLER_H

#include <WiFi.h>
#include "Globals.h"
#include "ImprovWiFiLibrary.h"
#include "LEDHandler.h"
#include "NonBlockingTimer.h"

class ImprovHandler
{
public:
    static void init();
    static void loop();

private:
    static ImprovWiFi improvSerial;
    static WiFiServer server;
    static NonBlockingTimer myTimer;
    static void handleHttpRequest();
    
    static void onImprovWiFiErrorCb(ImprovTypes::Error err);
    static void onImprovWiFiConnectedCb(const char *ssid, const char *password);
    static bool connectWifi(const char *ssid, const char *password);
};

#endif
