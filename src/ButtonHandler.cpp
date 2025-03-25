#ifdef ENABLE_BUTTON_HANDLER

#include "ButtonHandler.h"
#include "Globals.h"
#include "ConfigManager.h"
#include "CommandHandler.h"
#include "GfxHandler.h"
#include "LedHandler.h"
#include <OneButton.h>

// Configurable durations (in milliseconds)
const unsigned long REBOOT_HOLD_DURATION_MS = 5000; // 5 seconds total hold time to reboot
const unsigned long COUNTDOWN_START_MS = 2000; // Start countdown after 2 seconds (so countdown lasts 3 seconds)

// Ensure the countdown duration is valid
static_assert(REBOOT_HOLD_DURATION_MS > COUNTDOWN_START_MS, "REBOOT_HOLD_DURATION_MS must be greater than COUNTDOWN_START_MS");

// Static member initialization
unsigned long ButtonHandler::longPressStartTime = 0;
bool ButtonHandler::rebootTriggered = false;
int ButtonHandler::lastCountdownValue = -1;
static OneButton button(BUTTON_PIN, true);

// Initialize the button
void ButtonHandler::init()
{
    button.attachClick(handleSingleClick);
    button.attachDoubleClick(handleDoubleClick);
    button.attachLongPressStart(handleLongPress);
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
    debugI("Single click.");
    CommandHandler::handleCommand(settings.device.singlePress);
}

void ButtonHandler::handleDoubleClick()
{
    debugI("Double click.");
    CommandHandler::handleCommand(settings.device.doublePress);
}

void ButtonHandler::handleLongPress()
{
    debugI("Long press started.");
    CommandHandler::handleCommand(settings.device.longPress);
    longPressStartTime = millis(); // Record the start time of the long press
    rebootTriggered = false; // Reset the reboot flag
    lastCountdownValue = -1; // Reset the countdown value
}

void ButtonHandler::handleDuringLongPress()
{
    // Calculate how long the button has been held
    unsigned long currentTime = millis();
    unsigned long holdDuration = currentTime - longPressStartTime;

    debugI("During long press. Hold duration: %lu ms", holdDuration);

    // Check if we should start the countdown
    if (holdDuration >= COUNTDOWN_START_MS && !rebootTriggered) {
        // Calculate remaining time until reboot (in seconds)
        unsigned long remainingTimeMs = REBOOT_HOLD_DURATION_MS - holdDuration;
        int remainingSeconds = (remainingTimeMs + 999) / 1000; // Round up to the nearest second

        // Only update the display if the countdown value has changed
        if (remainingSeconds != lastCountdownValue && remainingSeconds >= 0) {
            lastCountdownValue = remainingSeconds;
            String message = "Rebooting in " + String(remainingSeconds) + " seconds...";
            GfxHandler::printMessage(message);
            LedHandler::setColorByName("orange");
            debugI("%s", message.c_str());
        }
    }

    // Check if the hold duration exceeds the reboot threshold and reboot hasn't been triggered yet
    if (holdDuration >= REBOOT_HOLD_DURATION_MS && !rebootTriggered) {
        debugI("Hold duration exceeded %lu ms. Rebooting device...", REBOOT_HOLD_DURATION_MS);
        rebootTriggered = true; // Set the flag to prevent multiple reboots
        LedHandler::setColorByName("purple");
        GfxHandler::printMessage("Rebooting now...");
        delay(1000); // Brief delay to ensure the message is displayed
        ESP.restart(); // Reboot the device (ESP32-specific)
    }
}

void ButtonHandler::handleLongPressStop()
{
    debugI("Long press stopped.");
    longPressStartTime = 0; // Reset the start time
    rebootTriggered = false; // Reset the reboot flag
    lastCountdownValue = -1; // Reset the countdown value
    GfxHandler::printMessage(""); // Clear the display message
}

#endif // ENABLE_BUTTON_HANDLER