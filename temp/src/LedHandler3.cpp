#ifdef ENABLE_LED_HANDLER

#include "LedHandler.h"

// Define static members
CRGB LedHandler::leds[NUM_LEDS];
uint8_t LedHandler::defaultBrightness = 100; // Default brightness

void LedHandler::init()
{
    FastLED.addLeds<LED_TYPE, LED_DI_PIN, LED_CI_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    clear(); // Turn off all LEDs initially
    // CommandHandler::registerCommand("led", runCommand);
    registerCommands();
    debugI("LEDHandler initialized with %d LED(s)", NUM_LEDS);
}

void LedHandler::setColor(const CRGB &color, uint8_t brightness)
{
    FastLED.setBrightness(brightness);
    leds[0] = color;
    FastLED.show();
    debugD("LED color set to R:%d G:%d B:%d with brightness %d", color.r, color.g, color.b, brightness);
}

void LedHandler::clear()
{
    FastLED.clear();
    FastLED.show();
    debugI("LEDs cleared");
}

void LedHandler::setDefaultBrightness(uint8_t brightness)
{
    defaultBrightness = brightness;
    FastLED.setBrightness(defaultBrightness);
    FastLED.show();
    debugI("Default brightness set to %d", brightness);
}

void LedHandler::setColorByName(const String &colorName, uint8_t brightness)
{
    String colorNameLower = colorName;
    colorNameLower.toLowerCase();

    auto it = colorMap.find(colorNameLower.c_str()); // Use c_str() for std::string lookup
    if (it != colorMap.end())
    {
        setColor(it->second, brightness);
    }
    else
    {
        debugW("Unknown color: %s", colorName.c_str());
    }
}

/*
void LedHandler::runCommand(const String &command) {
    String cmd = command;
    String args = "";

    int spaceIndex = command.indexOf(' ');
    if (spaceIndex > 0) {
        cmd = command.substring(0, spaceIndex);
        args = command.substring(spaceIndex + 1);
    }

    cmd.toLowerCase();

    if (cmd == "color") {
        setColorByName(args.c_str(), defaultBrightness);
    } else if (cmd == "clear") {
        clear();
    } else if (cmd == "brightness") {
        setDefaultBrightness(args.toInt());
    } else {
        debugW("Unknown command: %s", cmd.c_str());
    }
}
*/

void LedHandler::registerCommands()
{
    CommandHandler::registerCommand("LED", [](const String &command)
                                    {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (cmd == "color") {
            setColorByName(args.c_str(), defaultBrightness); // Replace 100 with your default brightness
        } else if (cmd == "clear") {
            clear();
        } else if (cmd == "brightness") {
            setDefaultBrightness(args.toInt());
        } else {
            debugW("Unknown LED subcommand: %s", cmd.c_str());
        } }, "Handles LED commands. Usage: led <subcommand> [args]\n"
                                         "  Subcommands:\n"
                                         "  color <color> - Set LED color by name (e.g., Red, Green, Blue)\n"
                                         "  clear - Clear all LEDs\n"
                                         "  brightness <level> - Set default brightness level (0-255)");
}

#endif // ENABLE_LED_HANDLER