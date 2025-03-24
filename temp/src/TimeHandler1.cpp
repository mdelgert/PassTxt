#ifdef ENABLE_TIME_HANDLER

#include "TimeHandler.h"
#include <time.h>           // for time(), struct tm, getLocalTime(), etc.
#include "NonBlockingTimer.h"

// Example: re-sync every 60 seconds (adjust as needed)
static NonBlockingTimer timeSyncTimer(60000);
static NonBlockingTimer timeTimer(1000);
static bool isTimeSynced = false;

// Adjust as needed for your timezone or keep 0 if you want UTC
// Example: UTC offset = 0, Daylight offset = 0
static const long  gmtOffset_sec      = 0; 
static const int   daylightOffset_sec = 0;

// Primary NTP server and fallback(s)
static const char* ntpServer1 = "pool.ntp.org";
static const char* ntpServer2 = "time.nist.gov";

void TimeHandler::init()
{
    debugI("TimeHandler initialized");

    // Configure time with NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

    // Optionally, you can do an immediate time sync check here if you like:
    syncTime();
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
