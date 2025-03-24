#include "LedHandler.h"

// Define static members
CRGB LedHandler::leds[NUM_LEDS];
uint8_t LedHandler::defaultBrightness = 100; // Default brightness

void LedHandler::init() {
    FastLED.addLeds<LED_TYPE, LED_DI_PIN, LED_CI_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    clear(); // Turn off all LEDs initially
    debugI("LEDHandler initialized with %d LED(s)", NUM_LEDS);
}

void LedHandler::setColor(const CRGB &color, uint8_t brightness) {
    FastLED.setBrightness(brightness);
    leds[0] = color;
    FastLED.show();
    debugD("LED color set to R:%d G:%d B:%d with brightness %d", color.r, color.g, color.b, brightness);
}

void LedHandler::clear() {
    FastLED.clear();
    FastLED.show();
    debugI("LEDs cleared");
}

void LedHandler::handleAction(const char *actionMessage) {
    if (strcmp(actionMessage, "Red") == 0) {
        setColor(CRGB::Red);
    } else if (strcmp(actionMessage, "Green") == 0) {
        setColor(CRGB::Green);
    } else if (strcmp(actionMessage, "Blue") == 0) {
        setColor(CRGB::Blue);
    } else if (strcmp(actionMessage, "Yellow") == 0) {
        setColor(CRGB::Yellow);
    } else if (strcmp(actionMessage, "White") == 0) {
        setColor(CRGB::White);
    } else if (strcmp(actionMessage, "Black") == 0) {
        setColor(CRGB::Black);
    } else {
        debugW("Unknown LED action: %s", actionMessage);
    }
}

void LedHandler::setDefaultBrightness(uint8_t brightness) {
    defaultBrightness = brightness;
    FastLED.setBrightness(defaultBrightness);
    FastLED.show();
    debugI("Default brightness set to %d", brightness);
}
