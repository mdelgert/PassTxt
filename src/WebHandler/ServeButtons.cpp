#ifdef ENABLE_WEB_HANDLER

#include "ServeButtons.h"
#include "Globals.h"
#include "WebHandler.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "CryptoHandler.h"

void ServeButtons::registerEndpoints(AsyncWebServer &server)
{
    server.on("/buttons", HTTP_GET, handleGetButtons);
    server.on("/buttons", HTTP_DELETE, handleDeleteButton);
    server.on("/buttons", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handlePostButtons);
}

void ServeButtons::handleGetButtons(AsyncWebServerRequest *request)
{
    File file = LittleFS.open(BUTTONS_FILE, "r");
    if (!file || file.size() == 0) {
        WebHandler::sendErrorResponse(request, 400, "Failed to read %s", BUTTONS_FILE);
        return;
    }

    String json = file.readString();
    file.close();

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", json);
    WebHandler::addCorsHeaders(response);
    request->send(response);
}

void ServeButtons::handleDeleteButton(AsyncWebServerRequest *request)
{
    if (!request->hasParam("id")) {
        WebHandler::sendErrorResponse(request, 400, "Missing button ID");
        return;
    }

    String buttonId = request->getParam("id")->value();
    File file = LittleFS.open(BUTTONS_FILE, "r");
    if (!file) {
        WebHandler::sendErrorResponse(request, 500, "Failed to read %s", BUTTONS_FILE);
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        WebHandler::sendErrorResponse(request, 500, "Invalid JSON format in %s", BUTTONS_FILE);
        return;
    }

    JsonArray buttons = doc["buttons"];
    bool buttonFound = false;

    for (size_t i = 0; i < buttons.size(); i++) {
        if (String(buttons[i]["id"].as<String>()) == buttonId) {
            buttons.remove(i);
            buttonFound = true;
            break;
        }
    }

    if (!buttonFound) {
        WebHandler::sendErrorResponse(request, 404, "Button ID not found");
        return;
    }

    file = LittleFS.open(BUTTONS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 500, "Failed to write %s", BUTTONS_FILE);
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();
    WebHandler::sendSuccessResponse(request, "Button deleted successfully");
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
        debugV("Opened existing file for reading: %s", BUTTONS_FILE);
        DeserializationError readError = deserializeJson(existingDoc, file);
        if (readError) {
            debugE("Failed to deserialize existing JSON: %s", readError.c_str());
            file.close();
            WebHandler::sendErrorResponse(request, 500, "Failed to read existing %s", BUTTONS_FILE);
            return;
        }
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
                debugV("Found button with ID: %d", existingButton["id"].as<int>());
                // Update only non-password fields, preserve existing password unless explicitly updating with plaintext
                for (JsonPair kv : newButton) {
                    if (kv.key() == "userPassword") {
                        // Only process password if explicitly marked as plaintext (e.g., via a flag or assumption)
                        // For now, assume encrypted passwords are sent as-is and skip decryption
                        if (kv.value().is<String>()) {
                            String newPassword = kv.value().as<String>();
                            if (!newPassword.isEmpty()) {
                                debugV("Preserving or updating userPassword as-is for ID: %d", existingButton["id"].as<int>());
                                existingButton["userPassword"] = newPassword; // Store as-is (encrypted)
                            } else {
                                debugV("Empty password skipped for ID: %d", existingButton["id"].as<int>());
                                continue; // Preserve existing password
                            }
                        }
                    } else {
                        debugV("Updating field '%s' to '%s' for ID: %d", kv.key().c_str(), kv.value().as<String>().c_str(), existingButton["id"].as<int>());
                        existingButton[kv.key()] = kv.value();
                    }
                }
                found = true;
                break;
            }
        }

        if (!found) {
            debugV("Creating new button");
            int maxId = 0;
            for (JsonObject existingButton : existingButtons) {
                int existingId = existingButton["id"] | 0;
                if (existingId > maxId) {
                    maxId = existingId;
                }
            }
            
            int newId = maxId + 1;
            
            JsonObject buttonToAdd = newButton;
            if (buttonToAdd["id"].isNull() || buttonToAdd["id"] == 0) {
                buttonToAdd["id"] = newId;
                if (buttonToAdd["userPassword"].is<String>()) {
                    String plainPassword = buttonToAdd["userPassword"].as<String>();
                    if (!plainPassword.isEmpty()) {
                        debugV("Encrypting password for new button ID: %d", newId);
                        String encryptedPassword = CryptoHandler::encryptAES(plainPassword, settings.device.userPassword);
                        buttonToAdd["userPassword"] = encryptedPassword;
                    } else {
                        debugV("Empty password removed for new button ID: %d", newId);
                        buttonToAdd.remove("userPassword");
                    }
                }
            }
            existingButtons.add(buttonToAdd);
            debugV("Added new button with ID: %d", newId);
        }
    }

    // Write the updated JSON back to the file
    debugV("Attempting to write updated JSON to %s", BUTTONS_FILE);
    file = LittleFS.open(BUTTONS_FILE, "w");
    if (!file) {
        debugE("Failed to open file for writing: %s", BUTTONS_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to write %s", BUTTONS_FILE);
        return;
    }

    debugV("Serializing updated JSON");
    serializeJsonPretty(existingDoc, file);
    debugV("Closing file after write");
    file.close();
    debugV("File write successful");
    WebHandler::sendSuccessResponse(request, "Buttons updated successfully");
}

#endif // ENABLE_WEB_HANDLER