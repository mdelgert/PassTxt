#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "Globals.h"
#include <OneButton.h>
#include "LedHandler.h"
#include "GfxHandler.h"

class ButtonHandler {
public:
    static void init();
    static void loop();

private:
    static OneButton button;
    static void handleSingleClick();
    static void handleDoubleClick();
    static void handleLongPress();
};

#endif // BUTTON_HANDLER_H