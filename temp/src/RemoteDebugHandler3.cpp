#ifdef ENABLE_REMOTE_DEBUG_HANDLER

#include "RemoteDebugHandler.h"

RemoteDebug Debug;

void RemoteDebugHandler::loop()
{
    Debug.handle(); // Handles RemoteDebug commands
    yield();        // Give a time for ESP
}

void RemoteDebugHandler::startNetwork()
{
    Debug.begin(settings.deviceName);
}

void RemoteDebugHandler::init()
{
    Serial.begin(115200);           // This might interfere with ImprovWifi because sending serial messages at the same time
    Debug.setSerialEnabled(true);   // Enable sending to serial debug as well (may want to disable it for release)
    //Debug.setResetCmdEnabled(true); // Enable the reset command
    //Debug.showTime(true);           // Show time in debug messages
    //Debug.showProfiler(true);       // Profiler (Good to measure times, to optimize codes)
    //Debug.showColors(true);         // Enable colors
    //Debug.setPassword("");     // Set the password for the debug console
    
    /*
    // Add custom commands
    Debug.setHelpProjectsCmds(
        "clear - Clear the terminal\n" // Clear is being implemented bry frontend javascript clear the window
        "help - Shows command handler help\n"
        //"hello - Print Hello, World!\n"
        //"tft [YourMessage] - Display a message on the TFT\n"
        //"led [YourColor] - Set LED color (e.g., Red, Green, Blue)"
    ); // Add help text for all commands
    */

    Debug.setCallBackProjectCmds(handleCustomCommands); // Use the global function as the callback
}

// Store the custom command handling logic in a global or class-level function
void RemoteDebugHandler::handleCustomCommands() {

    String command = Debug.getLastCommand(); // Retrieve the last received command
    CommandHandler::handleCommand(command); // Pass the command to the CommandHandler
    
    /*
    if (command.startsWith("clear")) {
        Debug.println("\033[2J\033[H"); // Send ANSI escape codes to clear the terminal
    } else if (command.startsWith("hello")) {
        Debug.println("Hello, World!"); // Print "Hello, World!" to the console
    } else if (command.startsWith("tft ")) { // Check if the command starts with "tft "
        String message = command.substring(4); // Extract the message after "tft "
        if (message.length() > 0) {
            GfxHandler::printMessage(message); // Call GfxHandler to print the message
            Debug.println("Message sent to TFT: " + message);
        } else {
            Debug.println("Error: No message provided. Usage: tft YourMessage");
        }
    } else if (command.startsWith("led ")) { // Check if the command starts with "led "
        String color = command.substring(4); // Extract the color after "led "
        if (color.length() > 0) {
            LedHandler::setColorByName(color.c_str());
            Debug.println("LED color set to: " + color);
        } else {
            Debug.println("Error: No color provided. Usage: led YourColor");
        }
    } else {
        Debug.println("Unknown command");
    }
    */
}
#endif // ENABLE_REMOTE_DEBUG_HANDLER