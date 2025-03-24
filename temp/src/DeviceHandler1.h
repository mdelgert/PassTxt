#pragma once

#ifdef ENABLE_DEVICE_HANDLER

#include "Globals.h"
#include <USB.h>
#include <USBHIDMouse.h>
#include <USBHIDKeyboard.h>

class DeviceHandler
{
private:
    static USBHIDMouse mouse;
    static USBHIDKeyboard keyboard;
    static void simulateMouseMovement();
    static void simulateKeyPress();

public:
    static void init();
    static void loop();
};

#else

class DeviceHandler
{
public: // No-op implementation of DeviceHandler
    static void loop() {} // No-op
    static void init() {} // No-op
};

#endif // ENABLE_DEVICE_HANDLER