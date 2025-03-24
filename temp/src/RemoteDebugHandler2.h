#ifndef REMOTE_DEBUG_HANDLER_H
#define REMOTE_DEBUG_HANDLER_H

#include "Globals.h"
#include <RemoteDebug.h>
#include "GfxHandler.h"
#include "LedHandler.h"

extern RemoteDebug Debug;

class RemoteDebugHandler
{
public:
    static void loop();
    static void startNetwork();
    static void init();
private:
    static void handleCustomCommands();
};

#endif
