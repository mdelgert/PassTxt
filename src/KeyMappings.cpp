#include "KeyMappings.h"
#include <algorithm>
#include <USBHIDKeyboard.h>

// Key mappings for script commands
const std::map<std::string, uint8_t> keyMap = {
    {"A", HID_KEY_A}, {"B", HID_KEY_B}, {"C", HID_KEY_C},
    {"D", HID_KEY_D}, {"E", HID_KEY_E}, {"F", HID_KEY_F},
    {"G", HID_KEY_G}, {"H", HID_KEY_H}, {"I", HID_KEY_I},
    {"J", HID_KEY_J}, {"K", HID_KEY_K}, {"L", HID_KEY_L},
    {"M", HID_KEY_M}, {"N", HID_KEY_N}, {"O", HID_KEY_O},
    {"P", HID_KEY_P}, {"Q", HID_KEY_Q}, {"R", HID_KEY_R},
    {"S", HID_KEY_S}, {"T", HID_KEY_T}, {"U", HID_KEY_U},
    {"V", HID_KEY_V}, {"W", HID_KEY_W}, {"X", HID_KEY_X},
    {"Y", HID_KEY_Y}, {"Z", HID_KEY_Z},

    {"a", HID_KEY_A}, {"b", HID_KEY_B}, {"c", HID_KEY_C},
    {"d", HID_KEY_D}, {"e", HID_KEY_E}, {"f", HID_KEY_F},
    {"g", HID_KEY_G}, {"h", HID_KEY_H}, {"i", HID_KEY_I},
    {"j", HID_KEY_J}, {"k", HID_KEY_K}, {"l", HID_KEY_L},
    {"m", HID_KEY_M}, {"n", HID_KEY_N}, {"o", HID_KEY_O},
    {"p", HID_KEY_P}, {"q", HID_KEY_Q}, {"r", HID_KEY_R},
    {"s", HID_KEY_S}, {"t", HID_KEY_T}, {"u", HID_KEY_U},
    {"v", HID_KEY_V}, {"w", HID_KEY_W}, {"x", HID_KEY_X},
    {"y", HID_KEY_Y}, {"z", HID_KEY_Z},

    {"1", HID_KEY_1}, {"2", HID_KEY_2}, {"3", HID_KEY_3},
    {"4", HID_KEY_4}, {"5", HID_KEY_5}, {"6", HID_KEY_6},
    {"7", HID_KEY_7}, {"8", HID_KEY_8}, {"9", HID_KEY_9},
    {"0", HID_KEY_0},

    {"ENTER", HID_KEY_ENTER},
    {"ESCAPE", HID_KEY_ESCAPE},
    {"ESC", HID_KEY_ESCAPE},
    {"BACKSPACE", HID_KEY_BACKSPACE},
    {"TAB", HID_KEY_TAB},
    {"SPACE", HID_KEY_SPACE},
    {"MINUS", HID_KEY_MINUS},
    {"EQUAL", HID_KEY_EQUAL},
    {"BRACKET_LEFT", HID_KEY_BRACKET_LEFT},
    {"BRACKET_RIGHT", HID_KEY_BRACKET_RIGHT},
    {"BACKSLASH", HID_KEY_BACKSLASH},
    {"SEMICOLON", HID_KEY_SEMICOLON},
    {"APOSTROPHE", HID_KEY_APOSTROPHE},
    {"GRAVE", HID_KEY_GRAVE},
    {"COMMA", HID_KEY_COMMA},
    {"PERIOD", HID_KEY_PERIOD},
    {"SLASH", HID_KEY_SLASH},
    {"CAPSLOCK", HID_KEY_CAPS_LOCK},
    
    {"F1", HID_KEY_F1}, {"F2", HID_KEY_F2}, {"F3", HID_KEY_F3},
    {"F4", HID_KEY_F4}, {"F5", HID_KEY_F5}, {"F6", HID_KEY_F6},
    {"F7", HID_KEY_F7}, {"F8", HID_KEY_F8}, {"F9", HID_KEY_F9},
    {"F10", HID_KEY_F10}, {"F11", HID_KEY_F11}, {"F12", HID_KEY_F12},

    {"ARROW_UP", HID_KEY_ARROW_UP},
    {"ARROW_DOWN", HID_KEY_ARROW_DOWN},
    {"ARROW_LEFT", HID_KEY_ARROW_LEFT},
    {"ARROW_RIGHT", HID_KEY_ARROW_RIGHT},
    {"CTRL", HID_KEY_CONTROL_LEFT},
    {"CONTROL", HID_KEY_CONTROL_LEFT},
    {"SHIFT", HID_KEY_SHIFT_LEFT},
    {"ALT", HID_KEY_ALT_LEFT},
    {"DELETE", HID_KEY_DELETE},
    {"END", HID_KEY_END},
    {"HOME", HID_KEY_HOME},
    {"INSERT", HID_KEY_INSERT},
    {"PAGE_UP", HID_KEY_PAGE_UP},
    {"PAGE_DOWN", HID_KEY_PAGE_DOWN},
    {"GUI", HID_KEY_GUI_LEFT},
    {"WINDOWS", HID_KEY_GUI_LEFT},
    {"GUI_RIGHT", HID_KEY_GUI_RIGHT}

    // Add more mappings as needed
};

// Optional: Mouse button mappings
const std::map<std::string, uint8_t> mouseButtonMap = {
    {"LEFT", 1},
    {"RIGHT", 2},
    {"MIDDLE", 3}
};

// Helper to get HID key code by name
uint8_t getKeyCode(const std::string& keyName) {
    auto it = keyMap.find(keyName);
    return (it != keyMap.end()) ? it->second : 0;  // Return 0 if not found
}

// Helper to get mouse button code by name
uint8_t getMouseButtonCode(const std::string& buttonName) {
    auto it = mouseButtonMap.find(buttonName);
    return (it != mouseButtonMap.end()) ? it->second : 0;  // Return 0 if not found
}
