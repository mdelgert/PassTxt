#include "ActionHandler.h"

static NonBlockingTimer delayTimer(500);

void ActionHandler::processMessage(const String &input)
{
  JsonDocument doc;
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
  if (!doc["action"].is<String>())
  {
    debugE("JSON does not contain a valid 'action' field.");
    return;
  }

  String action = doc["action"].as<String>();

  if (action == "reboot")
  {
    debugI("Action received: Rebooting system.");
    GfxHandler::printMessage("Rebooting...");

    if (delayTimer.isReady())
    {
      ESP.restart();
    }
  }
  else if (action == "tft")
  {
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
  else if (action == "led")
  {
    if (doc["message"].is<String>())
    {
      String message = doc["message"].as<String>();
      debugI("Action received: LED with message - %s", message.c_str());
      LEDHandler::handleAction(message.c_str());
    }
    else
    {
      debugE("TFT action requires a valid 'message' field.");
    }
  }
  else
  {
    debugE("Unknown action: %s", action.c_str());
  }
}
