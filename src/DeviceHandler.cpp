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

USBHIDKeyboard &DeviceHandler::getKeyboard() {
    return keyboard;
}

void DeviceHandler::setKeyPressDelay(int delay) {
    keyPressDelay = delay > 0 ? delay : 20; // Ensure delay is positive
    debugI("Key press delay set to %d ms", keyPressDelay);
}

void DeviceHandler::loop() {}

void DeviceHandler::init() {
    USB.begin();
    mouse.begin();
    keyboard.begin();
    registerCommands();
    debugI("DeviceHandler initialized");
}

void DeviceHandler::sendMouseMovement(int x, int y) {
    mouse.move(x, y);
    debugI("Mouse moved: x=%d, y=%d", x, y);
}

void DeviceHandler::sendKeys(const String &text) {
    for (size_t i = 0; i < text.length(); i++) {
        keyboard.write(text[i]);
        delay(keyPressDelay); // Configurable delay
    }
    debugI("Keys sent: %s", text.c_str());
}

void DeviceHandler::processKey(const String &keyName, bool press) {
    std::string key = keyName.c_str();
    auto it = KeyMappings::keyMap.find(key); // Fix: Use KeyMappings::keyMap
    if (it != KeyMappings::keyMap.end()) {
        uint8_t keyCode = it->second;
        if (press) {
            keyboard.pressRaw(keyCode); // Press the raw HID key code
            debugI("Pressed key: %s (code: %d)", keyName.c_str(), keyCode);
        } else {
            keyboard.releaseAll(); // Release all keys
            debugI("Released all keys");
        }
    } else {
        debugW("Invalid key: %s", keyName.c_str());
    }
}

void DeviceHandler::tapKey(const String &keyName) {
    processKey(keyName, true);  // Press
    delay(50);                  // Hold duration
    processKey(keyName, false); // Release
}

void DeviceHandler::registerCommands() {
    CommandHandler::registerCommand("hid", [](const String &command) {
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
                debugW("Invalid args for HID mouse. Expected: x,y");
            }
        }
        else if (cmd == "keys") {
            sendKeys(args);
        }
        else if (cmd == "winlock") {
            keyboard.pressRaw(HID_KEY_GUI_LEFT);
            keyboard.pressRaw(HID_KEY_L);
            delay(500);
            keyboard.releaseAll();
            debugI("Windows locked");
        }
        else if (cmd == "tapkey") {
            if (!args.isEmpty()) {
                DeviceHandler::tapKey(args);
            } else {
                debugW("No key provided for tapKey");
            }
        }
        else if (cmd == "processkey") {
            int spaceIndex = args.indexOf(' ');
            if (spaceIndex > 0) {
                String key = args.substring(0, spaceIndex);
                String action = args.substring(spaceIndex + 1);
                if (action.equalsIgnoreCase("press")) {
                    DeviceHandler::processKey(key, true);
                } else if (action.equalsIgnoreCase("release")) {
                    DeviceHandler::processKey(key, false);
                } else {
                    debugW("Invalid action for processKey. Use 'press' or 'release'");
                }
            } else {
                debugW("Invalid args for processKey. Expected: <key> <press/release>");
            }
        }
        else if (cmd == "delay") {
            int delay = args.toInt();
            if (delay > 0) {
                DeviceHandler::setKeyPressDelay(delay);
            } else {
                debugW("Invalid delay value. Expected positive integer");
            }
        }
        else {
            debugW("Unknown HID subcommand: %s", cmd.c_str());
        }
    },
    "Handles HID commands. Usage: hid <subcommand> [args]\n"
    "  Subcommands:\n"
    "  mouse x,y - Move mouse by x and y\n"
    "  keys <keys> - Send keys\n"
    "  winlock - Locks Windows\n"
    "  tapKey <key> - Tap a single key\n"
    "  processKey <key> <press/release> - Press or release a key\n"
    "  delay <ms> - Set key press delay (default 20 ms)");
}

#endif // ENABLE_DEVICE_HANDLER