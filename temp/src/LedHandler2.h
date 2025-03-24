#pragma once

#ifdef ENABLE_LED_HANDLER

#include "Globals.h"
#include <FastLED.h>
#include <unordered_map>
#include <string>

// LED configuration
#define NUM_LEDS 1
#define LED_TYPE APA102
#define COLOR_ORDER BGR

class LedHandler {
public:
    // Initialize the LED handler
    static void init();

    // Set LED color by name (e.g., "Red", "Blue")
    static void setColorByName(const std::string &colorName, uint8_t brightness = defaultBrightness);

    // Turn off all LEDs
    static void clear();

    // Set default brightness
    static void setDefaultBrightness(uint8_t brightness);

    // Run a command (e.g., "setColor Red")
    static void runCommand(const String &command);

private:
    static CRGB leds[NUM_LEDS];       // Static array for LED data
    static uint8_t defaultBrightness; // Default brightness level

    // Map of color names to CRGB values
    static const std::unordered_map<std::string, CRGB> colorMap;

    // Set LED color directly (internal use only)
    static void setColor(const CRGB &color, uint8_t brightness);
};

#else

#include <string>

// No-op implementation of LedHandler
class LedHandler {
public:
    static void init() {}                                // No-op
    static void setColorByName(const std::string &, uint8_t = 0) {} // No-op
    static void clear() {}                               // No-op
    static void setDefaultBrightness(uint8_t) {}         // No-op
    static void runCommand(const String &) {}            // No-op
};

#endif // ENABLE_LED_HANDLER