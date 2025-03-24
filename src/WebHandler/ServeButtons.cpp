#ifdef ENABLE_WEB_HANDLER

#include "ServeButtons.h"
#include "Globals.h"
#include "WebHandler.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

void ServeButtons::registerEndpoints(AsyncWebServer &server)
{
    server.on("/buttons", HTTP_GET, handleGetButtons);
    server.on("/buttons", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handlePostButtons);
    server.on("/buttons", HTTP_DELETE, handleDeleteButton);
}

void ServeButtons::handleGetButtons(AsyncWebServerRequest *request)
{
    // Open the file for reading
    File file = LittleFS.open(BUTTONS_FILE, "r");
    if (!file || file.size() == 0) {
        WebHandler::sendErrorResponse(request, 400, "Failed to read buttons2.json or file is empty");
        return;
    }

    String json = file.readString(); // Read the file content into a string
    file.close(); // Close the file

    //request->send(200, "application/json", json); // Send the file content as JSON
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", json);
    WebHandler::addCorsHeaders(response);
    request->send(response);
}

void ServeButtons::handlePostButtons(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /buttons");

    // Accumulate incoming data
    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start a new request body
    }
    requestBody += String((char *)data).substring(0, len); // Append the current chunk

    if (index + len != total) {
        return; // Wait until all chunks are received
    }

    debugV("Complete request body received: %s", requestBody.c_str());

    // Parse the incoming JSON payload
    JsonDocument incomingDoc;
    DeserializationError error = deserializeJson(incomingDoc, requestBody);

    if (error) {
        debugE("JSON deserialization failed: %s", error.c_str());
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON payload");
        return;
    }

    // Read the existing JSON file
    File file = LittleFS.open(BUTTONS_FILE, "r");
    JsonDocument existingDoc;
    
    if (file) {
        deserializeJson(existingDoc, file); // Deserialize existing data
        file.close();
    } else {
        debugW("No existing file found; initializing new document");
        existingDoc["buttons"] = JsonArray(); // Initialize an empty "buttons" array
    }

    JsonArray existingButtons = existingDoc["buttons"];
    JsonArray incomingButtons = incomingDoc["buttons"];

    // Update existing buttons or add new ones
    for (JsonObject newButton : incomingButtons) {
        bool found = false;

        for (JsonObject existingButton : existingButtons) {
            if (existingButton["id"] == newButton["id"]) {
                // Update only the provided fields for the matching button
                for (JsonPair kv : newButton) {
                    existingButton[kv.key()] = kv.value();
                }
                found = true;
                break;
            }
        }

        if (!found) {
            // Add new button
            existingButtons.add(newButton);
        }
    }

    // Write the updated JSON back to the file
    file = LittleFS.open(BUTTONS_FILE, "w");
    if (!file) {
        debugE("Failed to open file for writing: %s", BUTTONS_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to write buttons2.json");
        return;
    }

    //serializeJson(existingDoc, file); // Save updated JSON
    serializeJsonPretty(existingDoc, file); // Save updated JSON

    file.close();

    WebHandler::sendSuccessResponse(request, "Buttons updated successfully");
}

void ServeButtons::handleDeleteButton(AsyncWebServerRequest *request)
{
    // Check if the "id" parameter is provided
    if (!request->hasParam("id")) {
        WebHandler::sendErrorResponse(request, 400, "Missing button ID");
        return;
    }

    String buttonId = request->getParam("id")->value(); // Use String instead of int
    debugV("Attempting to delete button with ID: %s", buttonId.c_str());

    // Open the JSON file for reading
    File file = LittleFS.open(BUTTONS_FILE, "r");
    if (!file) {
        debugE("Failed to open file for reading: %s", BUTTONS_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to read buttons2.json");
        return;
    }

    // Parse the JSON content
    JsonDocument doc; // Adjust size as needed
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        debugE("Failed to parse JSON file: %s", error.c_str());
        WebHandler::sendErrorResponse(request, 500, "Invalid JSON format in buttons2.json");
        return;
    }

    JsonArray buttons = doc["buttons"];
    bool buttonFound = false;

    // Iterate over the buttons array and remove the one with the matching ID
    for (size_t i = 0; i < buttons.size(); i++) {
        if (String(buttons[i]["id"].as<String>()) == buttonId) { // Compare as strings
            buttons.remove(i);
            buttonFound = true;
            break;
        }
    }

    if (!buttonFound) {
        debugW("Button with ID %s not found", buttonId.c_str());
        WebHandler::sendErrorResponse(request, 404, "Button ID not found");
        return;
    }

    // Write the updated JSON back to the file
    file = LittleFS.open(BUTTONS_FILE, "w");
    if (!file) {
        debugE("Failed to open file for writing: %s", BUTTONS_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to write buttons2.json");
        return;
    }

    //serializeJson(doc, file);
    serializeJsonPretty(doc, file); // Save updated JSON

    file.close();

    debugV("Button with ID %s deleted successfully", buttonId.c_str());
    WebHandler::sendSuccessResponse(request, "Button deleted successfully");
}

/*
void ServeButtons::handleDeleteButton(AsyncWebServerRequest *request)
{
    // Check if the "id" parameter is provided
    if (!request->hasParam("id")) {
        WebHandler::sendErrorResponse(request, 400, "Missing button ID");
        return;
    }

    int buttonId = request->getParam("id")->value().toInt();
    debugV("Attempting to delete button with ID: %d", buttonId);

    // Open the JSON file for reading
    File file = LittleFS.open(BUTTONS_FILE, "r");
    if (!file) {
        debugE("Failed to open file for reading: %s", BUTTONS_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to read buttons2.json");
        return;
    }

    // Parse the JSON content
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        debugE("Failed to parse JSON file: %s", error.c_str());
        WebHandler::sendErrorResponse(request, 500, "Invalid JSON format in buttons2.json");
        return;
    }

    JsonArray buttons = doc["buttons"];
    bool buttonFound = false;

    // Iterate over the buttons array and remove the one with the matching ID
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i]["id"] == buttonId) {
            buttons.remove(i);
            buttonFound = true;
            break;
        }
    }

    if (!buttonFound) {
        debugW("Button with ID %d not found", buttonId);
        WebHandler::sendErrorResponse(request, 404, "Button ID not found");
        return;
    }

    // Write the updated JSON back to the file
    file = LittleFS.open(BUTTONS_FILE, "w");
    if (!file) {
        debugE("Failed to open file for writing: %s", BUTTONS_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to write buttons2.json");
        return;
    }

    serializeJson(doc, file);
    file.close();

    debugV("Button with ID %d deleted successfully", buttonId);
    WebHandler::sendSuccessResponse(request, "Button deleted successfully");
}
*/

#endif // ENABLE_WEB_HANDLER