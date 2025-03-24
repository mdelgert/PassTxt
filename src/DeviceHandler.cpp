#ifdef ENABLE_DEVICE_HANDLER

#include "DeviceHandler.h"
#include "Globals.h"
#include "DeviceDescriptors.h"
#include "KeyMappings.h"
#include <USB.h>

USBHIDMouse DeviceHandler::mouse;
USBHIDKeyboard DeviceHandler::keyboard;
TaskHandle_t sendKeysTaskHandle = nullptr;

int DeviceHandler::keyPressDelay = 20; // Default delay value

USBHIDKeyboard &DeviceHandler::getKeyboard()
{
    return keyboard;
}

void DeviceHandler::setKeyPressDelay(int delay)
{
    keyPressDelay = delay > 0 ? delay : 20; // Ensure the delay is positive
    debugI("Key press delay set to %d ms", keyPressDelay);
}

void DeviceHandler::loop() {}

void DeviceHandler::init()
{
    USB.begin();
    mouse.begin();
    keyboard.begin();
    registerCommands();
    debugI("DeviceHandler initialized");
}

void DeviceHandler::sendMouseMovement(int x, int y)
{
    // Move the mouse by the specified x and y values
    mouse.move(x, y);
    debugI("Mouse moved: x=%d, y=%d", x, y);
}

void DeviceHandler::sendKeys(const String &text)
{
    // Note: Both work with a delay between each key press. Does not work without a blocking delay.
    // Send the keys one by one with a delay
    for (size_t i = 0; i < text.length(); i++)
    {
        keyboard.write(text[i]);
        delay(keyPressDelay); // Use the configurable delay
    }
    keyboard.write('\n'); // Optional newline
    debugI("Keys sent: %s", text.c_str());
}

void DeviceHandler::processKey(const String &keyName, bool press) {
    // Convert Arduino String to std::string for map lookup
    std::string key = keyName.c_str();
    
    // Check if the key exists in the map
    auto it = keyMap.find(key);
    if (it != keyMap.end()) {
        uint8_t keyCode = it->second; // Get the mapped key code

        if (press) {
            keyboard.pressRaw(keyCode);  // Press the key using pressRaw
        } else {
            keyboard.releaseAll();      // Release all keys
        }
    } else {
        debugW("Invalid key: %s", keyName.c_str());
    }
}

void DeviceHandler::tapKey(const String &keyName) {
    // Use processKey to press and release the key
    processKey(keyName, true);  // Press the key
    delay(50);                  // Simulate key press duration
    processKey(keyName, false); // Release the key
}

void DeviceHandler::registerCommands()
{
    CommandHandler::registerCommand("hid", [](const String &command)
        {
            String cmd, args;
            CommandHandler::parseCommand(command, cmd, args);

            if (cmd == "mouse") {
                int x = 0, y = 0;
                int commaIndex = args.indexOf(',');
                if (commaIndex > 0) {
                    x = args.substring(0, commaIndex).toInt();
                    y = args.substring(commaIndex + 1).toInt();
                    DeviceHandler::sendMouseMovement(x, y);
                } else {
                    debugW("Invalid arguments for HID mouse. Expected format: x,y");
                }
            }
            else if (cmd == "keys") {
                sendKeys(args);
            }
            else if (cmd == "winlock") {
                keyboard.pressRaw(HID_KEY_GUI_LEFT);  // Press Windows key
                keyboard.pressRaw(HID_KEY_L);         // Press 'L'
                delay(500);
                keyboard.releaseAll();               // Release all keys
            }
            else if (cmd == "tapkey") {
                if (!args.isEmpty()) {
                    DeviceHandler::tapKey(args);  // Tap the specified key
                } else {
                    debugW("No key provided for tapKey command.");
                }
            }
            else if (cmd == "processkey") {
                int spaceIndex = args.indexOf(' ');
                if (spaceIndex > 0) {
                    String key = args.substring(0, spaceIndex);
                    String action = args.substring(spaceIndex + 1);

                    if (action.equalsIgnoreCase("press")) {
                        DeviceHandler::processKey(key, true);  // Press key
                    } else if (action.equalsIgnoreCase("release")) {
                        DeviceHandler::processKey(key, false);  // Release key
                    } else {
                        debugW("Invalid action for processKey. Use 'press' or 'release'.");
                    }
                } else {
                    debugW("Invalid arguments for processKey. Expected format: <key> <press/release>.");
                }
            }
            else if (cmd == "delay") {
                int delay = args.toInt();
                if (delay > 0) {
                    DeviceHandler::setKeyPressDelay(delay);
                    debugI("HID delay set to %d ms", delay);
                } else {
                    debugW("Invalid delay value. Expected a positive integer.");
                }
            }
            else {
                debugW("Unknown HID subcommand: %s", cmd.c_str());
            } },

            "Handles HID commands. Usage: hid <subcommand> [args]\n"
            "  Subcommands:\n"
            "  mouse x,y - Move mouse by x and y\n"
            "  keys <keys> - Send keys\n"
            "  winlock - Locks Windows\n"
            "  tapKey <key> - Tap a single key\n"
            "  processKey <key> <press/release> - Press or release a key\n"
            "  delay <ms> - Set the delay for key presses (default is 20 ms)"
            );
}

#endif // ENABLE_DEVICE_HANDLER