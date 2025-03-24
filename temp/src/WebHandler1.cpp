#include "WebHandler.h"

NonBlockingTimer WebHandler::myTimer(1000);
AsyncWebServer WebHandler::server(80);

void WebHandler::addCorsHeaders(AsyncWebServerResponse *response) {
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

    // Serve embedded index.html
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        size_t fileSize = index_html_end - index_html_start;
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response);
        debugI("Served: /index.html"); });

    /*
    // Serve embedded device.html
    server.on("/device.html", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        size_t fileSize = device_html_end - device_html_start;
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", device_html_start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response);
        debugI("Served: /device.html"); });

    server.on("/nav.html", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        size_t fileSize = nav_html_end - nav_html_start;
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", nav_html_start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response); 
        debugI("Served: /nav.html"); });

    // Serve embedded settings.html
    server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        size_t fileSize = settings_html_end - settings_html_start;
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", settings_html_start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response); 
        debugI("Served: /settings.html"); });

    // Serve embedded styles.css
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        size_t fileSize = styles_css_end - styles_css_start;
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", styles_css_start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response); 
        debugI("Served: /styles.css"); });

    // Serve embedded script.js
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        size_t fileSize = script_js_end - script_js_start;
        AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", script_js_start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response); 
        debugI("Served: /script.js"); });
    */

    // Serve embedded files
    serveEmbeddedFile("/", index_html_start, index_html_end, "text/html");
    serveEmbeddedFile("/device.html", device_html_start, device_html_end, "text/html");
    serveEmbeddedFile("/nav.html", nav_html_start, nav_html_end, "text/html");
    serveEmbeddedFile("/settings.html", settings_html_start, settings_html_end, "text/html");
    serveEmbeddedFile("/styles.css", styles_css_start, styles_css_end, "text/css");
    serveEmbeddedFile("/script.js", script_js_start, script_js_end, "application/javascript");

    // POST endpoint to accept JSON and save to LittleFS
    server.on("/save-json", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        debugI("Received JSON payload on /save-json");

        JsonDocument doc;

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

    server.on("/get-json", HTTP_GET, [](AsyncWebServerRequest *request) {
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

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
        WebHandler::addCorsHeaders(response);
        request->send(response);
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
