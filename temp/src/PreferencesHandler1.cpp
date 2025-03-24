#include "PreferencesHandler.h"

// Define static members
Preferences PreferencesHandler::preferences;
const char* PreferencesHandler::namespaceName = nullptr;
bool PreferencesHandler::initialized = false;

void PreferencesHandler::ensureInitialized() {
    if (!initialized) {
        preferences.begin(namespaceName, false);
        initialized = true;
        debugI("PreferencesHandler initialized with namespace: %s", namespaceName);
    }
}

void PreferencesHandler::init(const char* namespaceName) {
    PreferencesHandler::namespaceName = namespaceName;
    ensureInitialized();
}

void PreferencesHandler::clear() {
    ensureInitialized();
    preferences.clear();
    debugI("Preferences cleared for namespace: %s", namespaceName);
}

void PreferencesHandler::setValue(const String& key, const String& value) {
    ensureInitialized();
    preferences.putString(key.c_str(), value);
    debugI("Set key: %s to value: %s", key.c_str(), value.c_str());
}

// bool PreferencesHandler::getValue(const String& key, String& outValue) {
//     ensureInitialized();
//     outValue = preferences.getString(key.c_str(), "");
//     if (outValue.isEmpty()) {
//         debugW("Key: %s not found or has an empty value.", key.c_str());
//         return false;
//     }
//     debugI("Retrieved key: %s with value: %s", key.c_str(), outValue.c_str());
//     return true;
// }

bool PreferencesHandler::getValue(const String& key, String& outValue, const String& fallback) {
    ensureInitialized();
    outValue = preferences.getString(key.c_str(), "");
    if (outValue.isEmpty()) {
        debugW("Key: %s not found or has an empty value. Using fallback: %s", key.c_str(), fallback.c_str());
        outValue = fallback;
        return false; // Indicates fallback was used
    }
    debugI("Retrieved key: %s with value: %s", key.c_str(), outValue.c_str());
    return true; // Indicates value was found
}
