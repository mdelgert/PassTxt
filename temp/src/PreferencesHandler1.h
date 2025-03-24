#ifndef PREFERENCES_HANDLER_H
#define PREFERENCES_HANDLER_H

#include "Globals.h"
#include <Preferences.h>

class PreferencesHandler {
private:
    static Preferences preferences;          // Preferences object
    static const char* namespaceName;        // Namespace for all preferences
    static bool initialized;                 // Tracks if Preferences is initialized

    // Ensure Preferences are initialized
    static void ensureInitialized();

public:
    // Initialize Preferences
    static void init(const char* namespaceName);

    // Clear all stored keys
    static void clear();

    // Other methods like getValue and setValue can also be static if required
    static void setValue(const String& key, const String& value);
    static bool getValue(const String& key, String& outValue, const String& fallback);
};

#endif // PREFERENCES_HANDLER_H
