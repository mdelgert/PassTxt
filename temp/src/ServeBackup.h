#pragma once

#ifdef ENABLE_WEB_HANDLER

#include "Globals.h"
#include "WebHandler.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

class ServeBackup {
public:
    static void registerEndpoints(AsyncWebServer &server);

private:
    static void handleBackupRequest(AsyncWebServer &server);
    static void handleRestoreRequest(AsyncWebServer &server);
};

#endif // ENABLE_WEB_HANDLER
