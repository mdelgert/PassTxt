#ifdef ENABLE_TIME_HANDLER

#include "TimeHandler.h"
#include "Timezones.h"       // Include the timezone database
#include <time.h>            // For time(), struct tm, getLocalTime(), etc.
#include "NonBlockingTimer.h"

// Example: re-sync every 60 seconds (adjust as needed)
static NonBlockingTimer timeSyncTimer(60000);
static NonBlockingTimer timeTimer(1000);
static bool isTimeSynced = false;

// Hardcoded region for time zone
static const char* defaultRegion = "America/New_York";

// Primary NTP server and fallback(s)
static const char* ntpServer1 = "pool.ntp.org";
static const char* ntpServer2 = "time.nist.gov";

void TimeHandler::init()
{
    debugI("TimeHandler initialized");

    // Fetch POSIX string for the hardcoded region
    const char* posixTimeZone = tz_db_get_posix_str(defaultRegion);
    if (!posixTimeZone) {
        debugE("TimeHandler: Failed to find timezone for '%s'.", defaultRegion);
        return;
    }

    // Apply the POSIX string as the current time zone
    setenv("TZ", posixTimeZone, 1);
    tzset();
    debugI("TimeHandler: Timezone set to %s", posixTimeZone);

    // Configure time with NTP servers
    configTime(0, 0, ntpServer1, ntpServer2);
    syncTime(); // Optional immediate sync
}

void TimeHandler::loop()
{
    // If not yet synced at all, try again until success
    if (!isTimeSynced) {
        syncTime();
    }

    // Periodically re-sync to keep time accurate
    if (timeSyncTimer.isReady()) {
        debugI("TimeHandler: Resyncing time...");
        syncTime();
    }

    if (timeTimer.isReady()) {
        debugD("TimeHandler: Current time: %s", getFormattedTime());
    }
}

/**
 * syncTime() checks if the time is currently valid. If not, it tries to fetch and update.
 * Once successful, set isTimeSynced to true so we don't spam logs or re-do it excessively.
 */
void TimeHandler::syncTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        debugW("TimeHandler: Failed to obtain time");
        return;
    }
    // If this point is reached, we have valid time
    debugI("TimeHandler: Time successfully synchronized");
    isTimeSynced = true;
}

/**
 * Returns a string representing the current local time.
 * This function uses a static buffer for convenience; 
 * calling code should use or copy the result immediately.
 */
const char* TimeHandler::getFormattedTime()
{
    static char timeString[64];

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        strcpy(timeString, "Time not set");
        return timeString;
    }

    // Format: YYYY-MM-DD HH:MM:SS
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);

    return timeString;
}

#endif // ENABLE_TIME_HANDLER
