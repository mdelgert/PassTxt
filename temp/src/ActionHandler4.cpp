#include "ActionHandler.h"

static NonBlockingTimer delayTimer(500);

// Define an enumeration for actions
enum class ActionType {
    REBOOT,
    TFT,
    LED,
    UNKNOWN
};

// Function to map string to ActionType enum
ActionType getActionType(const String &actionStr) {
    if (actionStr == "reboot") {
        return ActionType::REBOOT;
    } else if (actionStr == "tft") {
        return ActionType::TFT;
    } else if (actionStr == "led") {
        return ActionType::LED;
    } else {
        return ActionType::UNKNOWN;
    }
}

void ActionHandler::processMessage(const String &input)
{
    JsonDocument doc; // Adjust size as needed
    DeserializationError error = deserializeJson(doc, input);

    if (error)
    {
        debugE("JSON deserialization failed: %s", error.c_str());
        return;
    }

    processMessage(doc);
}

void ActionHandler::processMessage(const JsonDocument &doc)
{
    // Replace containsKey("action") with a direct type check
    if (!doc["action"].is<String>())
    {
        debugE("JSON does not contain a valid 'action' field.");
        return;
    }

    String actionStr = doc["action"].as<String>();
    ActionType action = getActionType(actionStr);

    switch (action)
    {
        case ActionType::REBOOT:
            handleReboot();
            break;

        case ActionType::TFT:
            handleTft(doc);
            break;

        case ActionType::LED:
            handleLed(doc);
            break;

        default:
            debugE("Unknown action: %s", actionStr.c_str());
            break;
    }
}

void ActionHandler::handleReboot()
{
    debugI("Action received: Rebooting system.");
    GfxHandler::printMessage("Rebooting...");

    if (delayTimer.isReady())
    {
        ESP.restart();
    }
}

void ActionHandler::handleTft(const JsonDocument &doc)
{
    // Replace containsKey("message") with a direct type check
    if (doc["message"].is<String>())
    {
        String message = doc["message"].as<String>();
        debugI("Action received: TFT with message - %s", message.c_str());
        GfxHandler::printMessage(message);
    }
    else
    {
        debugE("TFT action requires a valid 'message' field.");
    }
}

void ActionHandler::handleLed(const JsonDocument &doc)
{
    // Replace containsKey("message") with a direct type check
    if (doc["message"].is<String>())
    {
        String message = doc["message"].as<String>();
        debugI("Action received: LED with message - %s", message.c_str());
        LedHandler::handleAction(message.c_str());
    }
    else
    {
        debugE("LED action requires a valid 'message' field.");
    }
}
