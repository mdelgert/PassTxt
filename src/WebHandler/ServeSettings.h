#pragma once

#ifdef ENABLE_WEB_HANDLER

#include <ESPAsyncWebServer.h>

class ServeSettings {
public:
    static void registerEndpoints(AsyncWebServer& server);

private:
    static void handleGetSettings(AsyncWebServer& server);
    static void handleSetSettings(AsyncWebServer& server);
    static void handleGetSettingsDeprecated(AsyncWebServerRequest *request);
    static void handleSetSettingsDeprecated(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif // ENABLE_WEB_HANDLER