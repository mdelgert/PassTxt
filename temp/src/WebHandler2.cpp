#include "WebHandler.h"

NonBlockingTimer WebHandler::myTimer(1000);
AsyncWebServer WebHandler::server(80);

void WebHandler::addCorsHeaders(AsyncWebServerResponse *response) {
    if (ENABLE_SECURE_CORS) return; // Only enable this for testing not safe for production
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
}

void WebHandler::serveEmbeddedFile(const char* path, const uint8_t* start, const uint8_t* end, const char* contentType) {
    server.on(path, HTTP_GET, [path, start, end, contentType](AsyncWebServerRequest* request) {
        size_t fileSize = end - start;
        AsyncWebServerResponse* response = request->beginResponse_P(200, contentType, start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response);
        debugI("Served: %s", path);
    });
}

void WebHandler::init()
{
    debugI("WebHandler initialized");

    while (WiFi.status() != WL_CONNECTED)
    {
        if (myTimer.isReady())
        {
            debugI("WebHandler waiting for WiFi...");
        }
    }

    // Initialize LittleFS
    if (!LittleFS.begin(true))
    {
        debugE("Failed to mount LittleFS");
        return;
    }
    debugI("LittleFS mounted successfully");

    // server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    //     request->redirect("/index.html");
    // });

    serveEmbeddedFile("/actions.html", actions_html_start, actions_html_end, "text/html");
    serveEmbeddedFile("/actions.js", actions_js_start, actions_js_end, "application/javascript");
    serveEmbeddedFile("/device.html", device_html_start, device_html_end, "text/html");
    //serveEmbeddedFile("/index.html", index_html_start, index_html_end, "text/html");
    serveEmbeddedFile("/", index_html_start, index_html_end, "text/html");
    serveEmbeddedFile("/settings.html", settings_html_start, settings_html_end, "text/html");
    serveEmbeddedFile("/settings.js", settings_js_start, settings_js_end, "application/javascript");
    serveEmbeddedFile("/styles.css", styles_css_start, styles_css_end, "text/css");
    
    // Serve files from LittleFS
    server.on("/settings/save", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        debugI("Received JSON payload on /settings/save");

        JsonDocument doc; // Use the simplified declaration

        // Parse the JSON data
        DeserializationError error = deserializeJson(doc, data, len);
        if (error) {
            debugE("Failed to parse JSON: %s", error.c_str());
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            return;
        }

        // Convert JSON to a string
        String jsonString;
        serializeJson(doc, jsonString);

        // Save JSON to LittleFS
        File file = LittleFS.open("/data.json", "w");
        if (!file) {
            debugE("Failed to open file for writing");
            request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save file\"}");
            return;
        }
        file.print(jsonString);
        file.close();

        debugI("JSON saved to /data.json");
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\",\"message\":\"File saved successfully\"}");
        WebHandler::addCorsHeaders(response);
        request->send(response);
    });
    
    // Serve JSON data
    server.on("/settings/get", HTTP_GET, [](AsyncWebServerRequest *request) {
        File file = LittleFS.open("/data.json", "r");
        if (!file) {
            debugE("Failed to open /data.json");
            AsyncWebServerResponse *response = request->beginResponse(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
            WebHandler::addCorsHeaders(response);
            request->send(response);
            return;
        }

        String json = file.readString();
        file.close();
        debugI("Serving /data.json");

        JsonDocument doc; // Use the simplified declaration
        DeserializationError error = deserializeJson(doc, json);
        if (error) {
            debugE("Failed to parse saved JSON: %s", error.c_str());
            request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Internal server error\"}");
            return;
        }

        // Convert JSON back to string for response
        String responseString;
        serializeJson(doc, responseString);

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", responseString);
        WebHandler::addCorsHeaders(response);
        request->send(response);
    });

    // Handle preflight CORS requests
    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->method() == HTTP_OPTIONS) {
            AsyncWebServerResponse *response = request->beginResponse(204); // No Content
            WebHandler::addCorsHeaders(response);
            request->send(response);
            debugI("Handled CORS preflight request");
        } else {
            request->send(404, "text/plain", "Not found");
        }
    });

    // Start the web server
    server.begin();
    debugI("Web server started!");
}

void WebHandler::loop()
{
    // No code needed for AsyncWebServer
    // debugI("WebHandler loop");
}
