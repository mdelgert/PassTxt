#pragma once

#define WIFI_NETWORKS_FILE "/data/networks.json"

//#define CATEGORIES_FILE "/data/categories.json"
#define CATEGORIES_FILE "/data/categoriesSecure.json"

//#define BUTTONS_FILE "/data/buttons.json"
#define BUTTONS_FILE "/data/buttonsSecure.json"

//#define SETTINGS_FILE "/data/settings.json"
#define SETTINGS_FILE "/data/settingsSecure.json"

#define EMQX_CERT_FILE "/data/mqtt1.crt"
#define TIMEZONES_FILE "/data/timezones.json"

// Custom USB descriptors
#define CUSTOM_PRODUCT_NAME "Passtxt USB KeyboardMouse"
#define CUSTOM_MANUFACTURER "Passtxt"
#define CUSTOM_SERIAL_NUMBER "123456789"

//#include "Secure.h"
#include "SecureExample.h"
#include "ConfigManager.h"
#include "NonBlockingTimer.h"
#include "RemoteDebugHandler.h"
#include "BoardPins.h"
#include "CommandHandler.h"