#pragma once

#ifdef ENABLE_BUTTON_HANDLER

// Full implementation of ButtonHandler
class ButtonHandler {
public:
    static void init();
    static void loop();

private:
    static void handleSingleClick();
    static void handleDoubleClick();
    static void handleLongPress();
};

#else
// No-op implementation of ButtonHandler
class ButtonHandler {
public:
    static void init() {} // No-op
    static void loop() {} // No-op
};

#endif // ENABLE_BUTTON_HANDLER
