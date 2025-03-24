#ifdef ENABLE_DEVICE_HANDLER

#include "DeviceHandler.h"

USBHIDMouse DeviceHandler::mouse;
USBHIDKeyboard DeviceHandler::keyboard;

static NonBlockingTimer mouseTimer(1000); // 1-second timer
static NonBlockingTimer keyboardTimer(500); // 500ms timer

void DeviceHandler::init()
{
    USB.begin();
    mouse.begin();
    keyboard.begin();
    debugI("DeviceHandler initialized");
}

void DeviceHandler::loop()
{
    if (mouseTimer.isReady())
    {
        simulateMouseMovement();
    }

    if (keyboardTimer.isReady())
    {
        simulateKeyPress();
    }
}

void DeviceHandler::simulateMouseMovement()
{
    // Example: Move mouse diagonally by 5 pixels
    mouse.move(5, 5);
    debugI("Mouse moved: x=5, y=5");
}

void DeviceHandler::simulateKeyPress()
{
    // Example: Send the "A" key press
    keyboard.press('A');
    keyboard.releaseAll();
    debugI("Key pressed: A");
}

#endif // ENABLE_DEVICE_HANDLER
