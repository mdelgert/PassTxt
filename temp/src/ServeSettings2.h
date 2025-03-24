#ifndef SERVE_SETTINGS_H
#define SERVE_SETTINGS_H

#include "Globals.h"
#include "WebHandler.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "PreferencesHandler.h"
#include "NonBlockingTimer.h"

class ServeSettings {
public:
    static void registerEndpoints(AsyncWebServer& server);

private:
    static void handleGetSettings(AsyncWebServer& server);
    static void handleSetSettings(AsyncWebServer& server);
};

#endif
