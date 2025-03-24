#include "ButtonHandler.h"

// Define static members
static NonBlockingTimer myTimer(500);
OneButton ButtonHandler::button(BUTTON_PIN, true);

// Initialize the button
void ButtonHandler::init()
{
    button.attachClick(handleSingleClick);
    button.attachDoubleClick(handleDoubleClick);
    button.attachLongPressStart(handleLongPress);
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

void ButtonHandler::handleLongPress()
{
    debugI("Long press detected.");
    GfxHandler::printMessage("Long press detected.");
    LedHandler::setColor(CRGB::Red);
    ConfigManager::clear();
    
    if (myTimer.isReady()) // Adding delay was rebooting before clearing preferences
    {
        ESP.restart();
    }
}
