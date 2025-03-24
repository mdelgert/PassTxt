//ropg/ezTime@0.8.3 - https://github.com/ropg/ezTime

#ifdef ENABLE_EZTIME_HANDLER

#include "EzTimeHandler.h"

static Timezone myTZ;
static bool isTimeSynced = false;
static char timeString[64];

// Example: re-sync every 60 seconds (adjust as needed).
static NonBlockingTimer syncTimer(60000);
static NonBlockingTimer syncDelay(1000);

void EzTimeHandler::init()
{
    debugI("EzTimeHandler initialized");

    // 1) Set your time zone location (handles DST automatically if region supports it).
    //    Check https://github.com/ropg/ezTime/blob/master/examples/Example13-Timezones/zoneList.txt
    //    for valid strings (e.g., "America/New_York", "Europe/Berlin", etc.).
    //myTZ.setLocation(F("America/New_York"));
    myTZ.setLocation(settings.timezone.c_str());

    // 2) Attempt an immediate time sync
    syncTime();

    // 3) Optionally, set the system time to the local epoch time
    if (syncDelay.isReady())
    {    
        time_t epoch = myTZ.now(); // local epoch
        settings.bootCount++;
        settings.bootTime = epoch;
        settings.upTime++;
        ConfigManager::save();
    }
}

void EzTimeHandler::loop()
{
    // ezTime needs events() called periodically
    events();

    // If time has never been synced, keep trying until success
    // This will flood the logs if not connected to WiFi, so be careful.
    // if (!isTimeSynced)
    // {
    //     syncTime();
    // }

    // Periodically re-sync using your NonBlockingTimer
    if (syncTimer.isReady())
    {
        debugI("EzTimeHandler: Performing periodic re-sync");
        syncTime();
    }

    if (syncDelay.isReady())
    {
        // debugI("EzTimeHandler: Local time is now: %s", getFormattedTime());
        // printExampleTimeFormats();
        GfxHandler::printMessage(myTZ.dateTime(F("H:i:s")).c_str());
        settings.upTime++;
        settings.currentTime = myTZ.dateTime(F("H:i:s")).c_str();
        settings.currentDate = myTZ.dateTime(F("m/d/Y")).c_str();
    }
}

/**
 * syncTime() calls ezTime's global updateNTP() (which has no return value).
 * We then check timeStatus() to see if time was set.
 */
void EzTimeHandler::syncTime()
{
    // updateNTP() triggers an NTP request in the background.
    // You can then check timeStatus() to see if it's timeSet, timeNeedsSync, or timeNotSet.
    updateNTP();

    if (timeStatus() == timeSet)
    {
        // We have valid time now.
        isTimeSynced = true;
        debugI("EzTimeHandler: NTP sync successful. Local time is now: %s", getFormattedTime());
        debugI("EzTimeHandler: DST active? %s", myTZ.isDST() ? "Yes" : "No");
        debugI("EzTimeHandler: UTC offset (seconds): %d", myTZ.getOffset());
    }
    else
    {
        debugW("EzTimeHandler: NTP sync attempt failed or pending (timeStatus != timeSet).");
    }
}

/**
 * Returns a string representing the current local time in the format "YYYY-MM-DD HH:MM:SS".
 */
const char *EzTimeHandler::getFormattedTime()
{
    if (!isTimeSynced)
    {
        strcpy(timeString, "Time not synced");
        return timeString;
    }

    // Format date/time with ezTime. For example: "Y-m-d H:i:s" => "2025-01-11 18:45:30"
    // You can customize the format using standard strftime-like tokens:
    //   https://github.com/ropg/ezTime#formatting-and-parsing
    String formatted = myTZ.dateTime(F("Y-m-d H:i:s"));
    strncpy(timeString, formatted.c_str(), sizeof(timeString));
    timeString[sizeof(timeString) - 1] = '\0'; // Ensure null termination

    return timeString;
}

/**
 * Optional helper function to print the time in multiple popular formats.
 */
void EzTimeHandler::printExampleTimeFormats()
{
    if (!isTimeSynced)
    {
        debugI("Time not synced, cannot display multiple formats.");
        return;
    }

    // 24-hour time (HH:MM:SS)
    debugI("24-hour time: %s", myTZ.dateTime(F("H:i:s")).c_str());
    // Example output: "14:05:09"

    // 12-hour format with AM/PM
    debugI("12-hour time: %s", myTZ.dateTime(F("h:i:s A")).c_str());
    // Example output: "02:05:09 PM"

    // Full date/time with weekday and time zone
    debugI("Full date/time: %s", myTZ.dateTime(F("l, F j Y H:i:s T")).c_str());
    // Example output: "Saturday, January 11 2025 14:05:09 EST"

    // Day/Month/Year in numeric form
    debugI("DD/MM/YYYY: %s", myTZ.dateTime(F("d/m/Y")).c_str());
    // Example output: "11/01/2025"

    // Another style: "Month day, year"
    debugI("Month day, year: %s", myTZ.dateTime(F("F j, Y")).c_str());
    // Example output: "January 11, 2025"

    // Print the Unix Epoch time (a.k.a. "Linux time")
    // myTZ.now() returns a time_t for local time,
    // but you can interpret it as "seconds since 1970" in UTC if you want raw epoch.
    time_t localEpoch = myTZ.now();
    // If you truly want UTC epoch, use `UTC.now()` or simply `::now()`
    // which is ezTime's global default if not set to a different Timezone.
    time_t utcEpoch = UTC.now();

    debugI("Local epoch time (seconds since 1970): %ld", (long)localEpoch);
    debugI("UTC epoch time: %ld", (long)utcEpoch);

    // For reference, the library also supports named constants like `ATOM`, `RFC822`, etc.
    // e.g., myTZ.dateTime(RFC822) => "Sat, 11 Jan 25 14:05:09 -0500"
}

#endif // ENABLE_EZTIME_HANDLER
