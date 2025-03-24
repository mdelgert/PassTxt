#pragma once

#include <Arduino.h>

class NonBlockingTimer
{
private:
    unsigned long previousMillis;
    unsigned long interval;

public:
    NonBlockingTimer(unsigned long interval) : previousMillis(0), interval(interval) {}

    void setInterval(unsigned long newInterval)
    {
        interval = newInterval;
    }

    bool isReady()
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            return true;
        }
        return false;
    }

    void reset()
    {
        previousMillis = millis();
    }
};

// Usage:
// #include "NonBlockingTimer.h"
// NonBlockingTimer myTimer(5000);

// if (myTimer.isReady())
// {
//     debugV("* This is a message of debug level VERBOSE");
//     debugD("* This is a message of debug level DEBUG");
//     debugI("* This is a message of debug level INFO");
//     debugW("* This is a message of debug level WARNING");
//     debugE("* This is a message of debug level ERROR");
// }