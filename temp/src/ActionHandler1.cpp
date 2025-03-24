#include "ActionHandler.h"

// Method to handle incoming actions from a JSON string
void ActionHandler::handleAction(const String& input) {
  // Parse the JSON input
  StaticJsonDocument<512> doc; // Specify a size based on your JSON structure
  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    debugE("JSON deserialization failed: %s", error.c_str());
    return;
  }

  // Delegate to the JsonDocument handler
  handleAction(doc);
}

// Method to handle incoming actions from a JsonDocument
void ActionHandler::handleAction(const JsonDocument& doc) {
  // Check if "action" exists and is a String
  if (!doc["action"].is<String>()) {
    debugE("JSON does not contain a valid 'action' field.");
    return;
  }

  String action = doc["action"].as<String>();

  // Handle actions using if-else statements
  if (action == "reboot") {
    debugI("Action received: Rebooting system.");
    // Add your reboot logic here
  }
  else if (action == "tft") {
    // Check if "message" exists and is a String
    if (doc["message"].is<String>()) {
      String message = doc["message"].as<String>();
      debugI("Action received: TFT with message - %s", message.c_str());
      // Add your TFT handling logic here
    }
    else {
      debugE("TFT action requires a valid 'message' field.");
    }
  }
  else {
    debugE("Unknown action: %s", action.c_str());
  }
}
