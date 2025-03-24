#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "Globals.h"
#include <OneButton.h>
#include "LedHandler.h"
#include "GfxHandler.h"
#include "ConfigManager.h"

class ButtonHandler {
public:
    // Initialize the button
    static void init();

    // Update the button state (call this in the main loop)
    static void loop();

private:
    // OneButton instance for handling button events
    static OneButton button;

    // Button action handlers
    static void handleSingleClick();
    static void handleDoubleClick();
    static void handleLongPressStart();
    static void handleDuringLongPress();
    static void handleLongPressStop();

    // Variables for custom long press actions
    static unsigned long holdStartTime;
    static bool countdownStarted;
};

#endif // BUTTON_HANDLER_H