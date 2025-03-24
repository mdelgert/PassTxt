#ifndef LED_HANDLER_H
#define LED_HANDLER_H

// #define FASTLED_INTERNAL
#include "Globals.h"
#include <FastLED.h>

// LED configuration
#define NUM_LEDS 1
#define LED_TYPE APA102
#define COLOR_ORDER BGR

class LedHandler
{
private:
    static CRGB leds[NUM_LEDS];       // Static array for LED data
    static uint8_t defaultBrightness; // Default brightness level

public:
    // Initialize the LED handler
    static void init();

    // Set LED color with optional brightness
    static void setColor(const CRGB &color, uint8_t brightness = defaultBrightness);

    // Turn off all LEDs
    static void clear();

    // Handle specific actions based on messages
    static void handleAction(const char *actionMessage);

    // Set default brightness
    static void setDefaultBrightness(uint8_t brightness);
};

#endif // LED_HANDLER_H
