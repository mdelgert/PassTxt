#include "ServeFiles.h"

void ServeFiles::registerEndpoints(AsyncWebServer &server) {
    server.on("/files", HTTP_GET, handleListFiles);           // List files
    server.on("/file", HTTP_GET, handleReadFile);             // Read a file
    server.on("/file", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleWriteFile); // Write a file
    server.on("/file", HTTP_DELETE, handleDeleteFile);        // Delete a file
}

void ServeFiles::handleListFiles(AsyncWebServerRequest *request) {
    debugV("Received GET request on /files");

    JsonDocument doc;
    JsonArray files = doc["files"].to<JsonArray>();

    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
        debugV("Found file: %s", file.name());
        files.add(file.name());
        file = root.openNextFile();
    }

    WebHandler::sendSuccessResponse(request, "Files listed successfully", &doc);
}

void ServeFiles::handleReadFile(AsyncWebServerRequest *request) {
    debugV("Received GET request on /file");

    if (!request->hasParam("filename")) {
        debugE("Missing filename parameter");
        WebHandler::sendErrorResponse(request, 400, "Filename is required");
        return;
    }
    String filename = request->getParam("filename")->value();
    debugV("Requested file: %s", filename.c_str());

    File file = LittleFS.open(filename, "r");
    if (!file) {
        debugE("File not found: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 404, "File not found");
        return;
    }

    String content = file.readString();
    file.close();
    debugV("File content: %s", content.c_str());

    request->send(200, "text/plain", content);
}

void ServeFiles::handleWriteFile(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    debugV("Received POST request on /file");

    if (!request->hasParam("filename")) {
        WebHandler::sendErrorResponse(request, 400, "Filename is required");
        return;
    }
    String filename = request->getParam("filename")->value();

    debugV("Filename: %s", filename.c_str());
    debugV("Data length: %d, Index: %d, Total: %d", len, index, total);

    File file = LittleFS.open(filename, index == 0 ? "w" : "a");
    if (!file) {
        debugE("Failed to open file for writing: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 500, "Failed to open file for writing");
        return;
    }

    file.write(data, len);
    file.close();
    debugV("Data written to file: %s", filename.c_str());

    if (index + len == total) {
        WebHandler::sendSuccessResponse(request, "File saved successfully");
    }
}

void ServeFiles::handleDeleteFile(AsyncWebServerRequest *request) {
    debugV("Received DELETE request on /file");

    if (!request->hasParam("filename")) {
        debugE("Missing filename parameter");
        WebHandler::sendErrorResponse(request, 400, "Filename is required");
        return;
    }
    String filename = request->getParam("filename")->value();
    debugV("Filename to delete: %s", filename.c_str());

    if (LittleFS.remove(filename)) {
        debugV("File deleted: %s", filename.c_str());
        WebHandler::sendSuccessResponse(request, "File deleted successfully");
    } else {
        debugE("Failed to delete file: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 500, "Failed to delete file");
    }
}
