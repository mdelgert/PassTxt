#ifndef CRON_HANDLER_H
#define CRON_HANDLER_H

#include <Arduino.h>
#include <vector>
#include <time.h>
#include "CommandHandler.h"

// Structure to represent a Cron Job
struct CronJob {
    String command;                // Raw command to execute
    String cronExpression;         // Cron expression string
    bool repeat;                   // Whether the job repeats

    CronJob(const String& cmd, const String& expr, bool r)
        : command(cmd), cronExpression(expr), repeat(r) {}
};

class CronHandler {
public:
    static void init();
    static void addCronJob(const String& command, const String& cronExpression, bool repeat = true);
    static void removeCronJob(size_t index);
    static void listCronJobs();
    static void checkCronJobs();

private:
    static std::vector<CronJob> cronJobs;
    static const size_t MAX_CRON_JOBS;

    // Helper functions
    static bool matchesCronField(const String& field, int currentValue);
    static bool parseCronExpression(const String& cronExpression, struct tm* currentTime);

    // Utility function to split a String
    static std::vector<String> split(const String& str, char delimiter);
};

#endif
