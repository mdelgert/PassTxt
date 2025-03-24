#ifndef KEY_MAPPINGS_H
#define KEY_MAPPINGS_H

#include <map>
#include <string>

// Map for friendly command names to HID key codes
extern const std::map<std::string, uint8_t> keyMap;

// Map for mouse buttons (optional, if needed)
extern const std::map<std::string, uint8_t> mouseButtonMap;

// Utility function to get HID key code by name
uint8_t getKeyCode(const std::string& keyName);

// Utility function to get mouse button code by name
uint8_t getMouseButtonCode(const std::string& buttonName);

#endif // KEY_MAPPINGS_H
