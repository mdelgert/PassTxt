#ifdef ENABLE_LED_HANDLER

#include "LedHandler.h"

// Define static members
CRGB LedHandler::leds[NUM_LEDS];
uint8_t LedHandler::defaultBrightness = 100; // Default brightness

// Define the color map
const std::unordered_map<std::string, CRGB> LedHandler::colorMap = {
    {"Red", CRGB::Red},
    {"Green", CRGB::Green},
    {"Blue", CRGB::Blue},
    {"Yellow", CRGB::Yellow},
    {"White", CRGB::White},
    {"Black", CRGB::Black},
    {"Orange", CRGB::Orange},
    {"Purple", CRGB::Purple},
    {"Pink", CRGB::Pink},
    {"Cyan", CRGB::Cyan},
    {"Magenta", CRGB::Magenta},
    {"Brown", CRGB::Brown},
    {"Lime", CRGB::Lime},
    {"Turquoise", CRGB::Turquoise},
    {"Violet", CRGB::Violet},
    {"Gold", CRGB::Gold},
    {"Silver", CRGB::Silver},
    {"Teal", CRGB::Teal},
    {"Navy", CRGB::Navy},
    {"Maroon", CRGB::Maroon},
    {"Olive", CRGB::Olive},
    {"SkyBlue", CRGB::SkyBlue},
    {"HotPink", CRGB::HotPink},
    {"Chartreuse", CRGB::Chartreuse},
    {"Aquamarine", CRGB::Aquamarine},
    {"Khaki", CRGB::Khaki},
    {"Lavender", CRGB::Lavender},
    {"Beige", CRGB::Beige}
};

void LedHandler::init() {
    FastLED.addLeds<LED_TYPE, LED_DI_PIN, LED_CI_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    clear(); // Turn off all LEDs initially
    CommandHandler::registerCommand("led", runCommand);
    debugI("LEDHandler initialized with %d LED(s)", NUM_LEDS);
}

void LedHandler::setColor(const CRGB &color, uint8_t brightness) {
    FastLED.setBrightness(brightness);
    leds[0] = color;
    FastLED.show();
    debugD("LED color set to R:%d G:%d B:%d with brightness %d", color.r, color.g, color.b, brightness);
}

void LedHandler::setColorByName(const std::string &colorName, uint8_t brightness) {
    auto it = colorMap.find(colorName);
    if (it != colorMap.end()) {
        setColor(it->second, brightness);
    } else {
        debugW("Unknown color: %s", colorName.c_str());
    }
}

void LedHandler::clear() {
    FastLED.clear();
    FastLED.show();
    debugI("LEDs cleared");
}

void LedHandler::setDefaultBrightness(uint8_t brightness) {
    defaultBrightness = brightness;
    FastLED.setBrightness(defaultBrightness);
    FastLED.show();
    debugI("Default brightness set to %d", brightness);
}

void LedHandler::runCommand(const String &command) {
    String cmd = command;
    String args = "";

    int spaceIndex = command.indexOf(' ');
    if (spaceIndex > 0) {
        cmd = command.substring(0, spaceIndex);
        args = command.substring(spaceIndex + 1);
    }

    if (cmd == "setColor") {
        setColorByName(args.c_str(), defaultBrightness);
    } else if (cmd == "clear") {
        clear();
    } else if (cmd == "setDefaultBrightness") {
        setDefaultBrightness(args.toInt());
    } else {
        debugW("Unknown command: %s", cmd.c_str());
    }
}

#endif // ENABLE_LED_HANDLER