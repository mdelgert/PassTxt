#include "ButtonHandler.h"

// Define static members
OneButton ButtonHandler::button(BUTTON_PIN, true);
unsigned long ButtonHandler::holdStartTime = 0; // Initialize to 0
bool ButtonHandler::countdownStarted = false;   // Initialize to false

// Variables for tracking long press duration
unsigned long holdStartTime = 0;
bool countdownStarted = false;

// Initialize the button
void ButtonHandler::init()
{
    button.attachClick(handleSingleClick);
    button.attachDoubleClick(handleDoubleClick);
    button.attachLongPressStart(handleLongPressStart);
    button.attachDuringLongPress(handleDuringLongPress);
    button.attachLongPressStop(handleLongPressStop);
    debugI("ButtonHandler initialized on pin: %d", BUTTON_PIN);
}

// Update the button state (call in loop)
void ButtonHandler::loop()
{
    button.tick();
}

// Button action handlers
void ButtonHandler::handleSingleClick()
{
    debugI("Single click detected.");
    GfxHandler::printMessage("Single click detected.");
    LedHandler::setColor(CRGB::Blue);
}

void ButtonHandler::handleDoubleClick()
{
    debugI("Double click detected.");
    GfxHandler::printMessage("Double click detected.");
    LedHandler::setColor(CRGB::Green);
}

void ButtonHandler::handleLongPressStart()
{
    debugI("Long press detected.");
    GfxHandler::printMessage("Long press detected. Hold for 5 seconds to reboot.");
    LedHandler::setColor(CRGB::Red);
    ConfigManager::clear();
    delay(3000);                 // Adding delay appear like it reboots before clearing preferences
    ESP.restart();
    //holdStartTime = millis(); // Start timing
    //countdownStarted = false; // Reset countdown flag
}

void ButtonHandler::handleDuringLongPress()
{
    unsigned long elapsed = millis() - holdStartTime;

    if (elapsed >= 1000 && !countdownStarted)
    {
        countdownStarted = true; // Start countdown
        for (int i = 5; i > 0; --i)
        {
            LedHandler::setColor(CRGB::Orange);
            GfxHandler::printMessage("Rebooting in " + String(i) + " seconds...");
            delay(1000); // Display countdown (1-second intervals)
        }
    }

    if (elapsed >= 50000)
    { 
        LedHandler::setColor(CRGB::Red);
        GfxHandler::printMessage("Rebooting device...");
        debugI("Rebooting device...");
        delay(1000);   // Optional delay before reboot
        ESP.restart(); // Reboot the ESP32
    }
}

void ButtonHandler::handleLongPressStop()
{
    debugI("Long press stopped.");
    GfxHandler::printMessage("Long press released. Action cancelled.");
    holdStartTime = 0;        // Reset timing
    countdownStarted = false; // Reset countdown flag
}
