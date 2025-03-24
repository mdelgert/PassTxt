#pragma once

#include "Globals.h"
#include <ArduinoJson.h>
#include "GfxHandler.h"
#include "LedHandler.h"

class ActionHandler {

public:
    static void processMessage(const String& input);
    static void processMessage(const JsonDocument& doc);
private:
    static void handleReboot();
    static void handleTft(const JsonDocument& doc);
    static void handleLed(const JsonDocument& doc);
};

/* Example usage:

#include "ActionHandler.h"

    // Example JSON as a string
    String jsonMessage = R"({
        "action": "reboot"
    })";

    // Call ActionHandler with the JSON string
    ActionHandler::processMessage(jsonMessage);

    // Example JSON
    JsonDocument doc;
    doc["action"] = "tft";
    doc["message"] = "Hello!";

    // Call ActionHandler with the JsonDocument
    ActionHandler::processMessage(doc);

*/

