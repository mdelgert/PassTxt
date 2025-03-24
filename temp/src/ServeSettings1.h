#ifndef SERVE_SETTINGS_H
#define SERVE_SETTINGS_H

#include "Globals.h"
#include "WebHandler.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "PreferencesHandler.h"

class ServeSettings {
public:
    // Register endpoints with the provided server instance
    static void registerEndpoints(AsyncWebServer &server);

private:
    // Handler for GET /settings/get
    static void handleGetSettings(AsyncWebServerRequest *request);

    // Handler for POST /settings/set
    static void handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif
