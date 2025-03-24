#pragma once

#include <Arduino.h>

class NonBlockingTimer
{
private:
    unsigned long previousMillis;
    unsigned long interval;
    unsigned long pausedAt = 0;

public:
    NonBlockingTimer(unsigned long interval) : interval(interval) { reset(); }
    NonBlockingTimer() : previousMillis(0), interval(0) {}

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

    unsigned long remaining()
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            return 0; // Timer has expired
        }
        return interval - (currentMillis - previousMillis);
    }

    void stop()
    {
        previousMillis = millis() + interval; // Effectively disables the timer
    }

    bool isRunning()
    {
        return remaining() > 0;
    }

    void pause()
    {
        pausedAt = millis() - previousMillis;
        previousMillis = 0; // Mark as paused
    }

    void resume()
    {
        if (pausedAt > 0)
        {
            previousMillis = millis() - pausedAt;
            pausedAt = 0;
        }
    }

    void debugInfo()
    {
        Serial.print("Timer Info: Interval=");
        Serial.print(interval);
        Serial.print(" ms, Remaining=");
        Serial.print(remaining());
        Serial.print(" ms, PreviousMillis=");
        Serial.println(previousMillis);
    }
};
