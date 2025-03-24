#ifdef ENABLE_TIME_HANDLER

#include "TimeHandler.h"

// Timers for resync and periodic updates
//600000 = 10 minutes, 3600000 = 1 hour, 28800000 = 8 hours and 86400000 = 1 day
static NonBlockingTimer timeSyncTimer(3600000); // 1 hour
static NonBlockingTimer timeTimer(1000);
static bool isTimeSynced = false;

// NTP servers
//The public rate limit (without an API key) is 5 requests in a rolling 30 second window; 
//the rate limit with an API key is 50 requests in a rolling 30 second window.
//A query interval of 10 minutes or longer is generally acceptable for both services.
static const char *ntpServer1 = "pool.ntp.org";
static const char *ntpServer2 = "time.nist.gov";

// Dynamically set offsets
static long gmtOffset_sec = 0;
static int daylightOffset_sec = 0;

// Default region hardcoded
const char* defaultRegion = "Etc/Zulu";

void TimeHandler::init()
{
    debugI("TimeHandler initialized");

    if (settings.timezone && strlen(settings.timezone.c_str()) > 0) {
        defaultRegion = settings.timezone.c_str();
        debugI("TimeHandler: Using timezone from settings: %s", defaultRegion);
    } else {
        debugW("TimeHandler: No timezone set. Using default: %s", defaultRegion);
    }

    debugI("TimeHandler: Using timezone: %s", defaultRegion);

    // Get POSIX string for the default region
    const char *posix_str = tz_db_get_posix_str(defaultRegion);
    if (!posix_str)
    {
        debugE("TimeHandler: Failed to find POSIX string for region %s", defaultRegion);
        return;
    }
    debugI("TimeHandler: Found POSIX string: %s", posix_str);

    // Parse offsets
    if (!parsePosixOffsets(posix_str, &gmtOffset_sec, &daylightOffset_sec))
    {
        debugE("TimeHandler: Failed to parse offsets from POSIX string: %s", posix_str);
        return;
    }
    debugI("TimeHandler: Parsed offsets - gmtOffset_sec: %ld, daylightOffset_sec: %d", gmtOffset_sec, daylightOffset_sec);

    // Configure time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
    syncTime();

    // Update boot count and time
    settings.bootCount++;
    settings.bootTime = time(nullptr);
    settings.upTime++;
    ConfigManager::save();
}

void TimeHandler::loop()
{
    if (!isTimeSynced)
    {
        syncTime();
    }

    if (timeSyncTimer.isReady())
    {
        debugI("TimeHandler: Resyncing time...");
        syncTime();
    }

    if (timeTimer.isReady())
    {
        //logAllDateTimeFormats();
        //GfxHandler::printMessage(formatDateTime("%I:%M:%S %p"));
        settings.upTime++;
        settings.currentTime = formatDateTime("%I:%M:%S %p");
        settings.currentDate = formatDateTime("%m-%d-%Y");
    }
}

void TimeHandler::syncTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        debugW("TimeHandler: Failed to obtain time");
        return;
    }

    debugI("TimeHandler: Time successfully synchronized. Current local time: %s", formatDateTime("%Y-%m-%d %I:%M:%S %p"));
    //debugD("TimeHandler: DST flag (tm_isdst): %d", timeinfo.tm_isdst);

    isTimeSynced = true;
}

/**
 * Parses the gmtOffset_sec and daylightOffset_sec from a POSIX timezone string.
 * Example POSIX string: "EST5EDT,M3.2.0,M11.1.0"
 */
bool TimeHandler::parsePosixOffsets(const char *posixStr, long *gmtOffset, int *daylightOffset)
{
    if (!posixStr || !gmtOffset || !daylightOffset)
    {
        return false;
    }

    // Parse the GMT offset (e.g., "EST5" -> 5 * 3600)
    const char *gmtStart = strchr(posixStr, 'T');
    if (!gmtStart)
    {
        return false;
    }
    *gmtOffset = -(atoi(gmtStart + 1) * 3600); // Negative because POSIX strings specify offsets as positive

    // Look for daylight saving offset (EDT)
    const char *daylightStart = strchr(posixStr, ',');
    if (daylightStart)
    {
        *daylightOffset = 3600; // Default to 1 hour if DST is specified
    }
    else
    {
        *daylightOffset = 0; // No DST
    }

    return true;
}

long TimeHandler::getLinuxTime()
{
    return time(nullptr); // Returns the current time as seconds since epoch
}

const char* TimeHandler::formatDateTime(const char* format) {
    static char buffer[64]; // Reusable buffer for formatted output
    struct tm timeinfo;
    time_t now = time(nullptr); // Get current epoch time

    if (!getLocalTime(&timeinfo)) {
        return "Time not available";
    }

    strftime(buffer, sizeof(buffer), format, &timeinfo);
    return buffer;
}

void TimeHandler::logAllDateTimeFormats()
{
    // Full Date-Time (12-hour format with AM/PM)
    debugI("Full Date-Time (12-hour): %s", formatDateTime("%Y-%m-%d %I:%M:%S %p"));

    // Full Date-Time (24-hour format)
    debugI("Full Date-Time (24-hour): %s", formatDateTime("%Y-%m-%d %H:%M:%S"));

    // Date Only
    debugI("Date Only: %s", formatDateTime("%Y-%m-%d"));

    // Time Only (12-hour format with AM/PM)
    debugI("Time Only (12-hour): %s", formatDateTime("%I:%M:%S %p"));

    // Time Only (24-hour format)
    debugI("Time Only (24-hour): %s", formatDateTime("%H:%M:%S"));

    // Day of the Week
    debugI("Day of the Week: %s", formatDateTime("%A"));

    // Month and Year
    debugI("Month and Year: %s", formatDateTime("%B %Y"));

    // Weekday, Month, and Date
    debugI("Weekday, Month, Date: %s", formatDateTime("%A, %B %d"));

    // Short Date-Time (MM/DD/YY HH:MM)
    debugI("Short Date-Time: %s", formatDateTime("%m/%d/%y %H:%M"));

    // ISO 8601 Format
    debugI("ISO 8601 Format: %s", formatDateTime("%Y-%m-%dT%H:%M:%S"));

    // Linux Epoch Time
    debugI("Linux Epoch Time: %ld", time(nullptr));

    // Custom Timezone String
    debugI("Custom Timezone: %s", formatDateTime("%Z"));
}

#endif // ENABLE_TIME_HANDLER
