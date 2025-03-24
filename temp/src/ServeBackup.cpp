#ifdef ENABLE_WEB_HANDLER

#include "ServeBackup.h"

void ServeBackup::registerEndpoints(AsyncWebServer &server)
{
    handleBackupRequest(server);
    handleRestoreRequest(server);
}

void ServeBackup::handleBackupRequest(AsyncWebServer &server)
{
    server.on("/backup", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        debugV("Received GET request on /backup");

        AsyncWebServerResponse *response = request->beginChunkedResponse("application/x-tar",
            [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
                static File root;
                static File currentFile;
                static bool allFilesSent = false;
                static size_t fileBytesSent = 0;

                if (index == 0) {
                    root = LittleFS.open("/");
                    currentFile = File();
                    allFilesSent = false;
                    fileBytesSent = 0;
                }

                if (allFilesSent) {
                    return 0;
                }

                while (!currentFile || !currentFile.available()) {
                    if (currentFile) {
                        currentFile.close();
                    }
                    currentFile = root.openNextFile();
                    if (!currentFile) {
                        allFilesSent = true;
                        return 0;
                    }

                    // Tar header creation
                    char header[512] = {0};
                    snprintf(header, 100, "%s", currentFile.name() + 1); // File name without '/'
                    snprintf(header + 124, 12, "%011o", (unsigned)currentFile.size()); // File size in octal
                    memset(header + 257, 'u', 6); // Tar format
                    memcpy(buffer, header, 512);
                    return 512; // Send header
                }

                size_t bytesToRead = currentFile.size() - fileBytesSent;
                bytesToRead = bytesToRead > maxLen ? maxLen : bytesToRead;
                size_t bytesRead = currentFile.read(buffer, bytesToRead);
                fileBytesSent += bytesRead;

                // Pad file data to a 512-byte block
                if (fileBytesSent == currentFile.size()) {
                    currentFile.close();
                    size_t padding = (512 - (fileBytesSent % 512)) % 512;
                    memset(buffer + bytesRead, 0, padding);
                    return bytesRead + padding;
                }

                return bytesRead;
            });

        response->addHeader("Content-Disposition", "attachment; filename=backup.tar");
        WebHandler::addCorsHeaders(response);
        request->send(response);
    });
}

void ServeBackup::handleRestoreRequest(AsyncWebServer &server)
{
    server.on("/restore", HTTP_POST,
              [](AsyncWebServerRequest *request)
              {
                  debugV("Restore request completed");
                  WebHandler::sendSuccessResponse(request, "Restore completed");
              },
              NULL,
              [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              {
                  static String currentFileName;
                  static File currentFile;
                  static size_t currentFileSize = 0;
                  static size_t totalBytesWritten = 0;

                  if (index == 0) {
                      debugV("Starting tar upload...");
                  }

                  size_t offset = 0;
                  while (offset < len) {
                      // Parse tar header (512 bytes)
                      if (currentFileSize == 0 && len - offset >= 512) {
                          char header[512];
                          memcpy(header, data + offset, 512);
                          offset += 512;

                          // Check for empty header (end of tar)
                          if (header[0] == '\0') {
                              break;
                          }

                          // Extract file information
                          currentFileName = String(header);
                          currentFileSize = strtol(header + 124, nullptr, 8); // File size in octal
                          debugV("Extracting: %s (%d bytes)", currentFileName.c_str(), currentFileSize);

                          // Open file for writing
                          currentFile = LittleFS.open("/" + currentFileName, "w");
                          if (!currentFile) {
                              debugE("Failed to open file for writing: %s", currentFileName.c_str());
                              WebHandler::sendErrorResponse(request, 500, "File write error");
                              return;
                          }
                      }

                      // Write file data
                      if (currentFileSize > 0) {
                          size_t bytesToWrite = min(currentFileSize, len - offset);
                          currentFile.write(data + offset, bytesToWrite);
                          offset += bytesToWrite;
                          currentFileSize -= bytesToWrite;
                          totalBytesWritten += bytesToWrite;

                          // Close file if done
                          if (currentFileSize == 0) {
                              currentFile.close();
                              debugV("File %s written successfully", currentFileName.c_str());
                          }
                      }
                  }

                  if (index + len == total) {
                      debugV("Tar upload completed: %d bytes written", totalBytesWritten);
                  }
              });
}

#endif // ENABLE_WEB_HANDLER
