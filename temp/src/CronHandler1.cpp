#include "CronHandler.h"

std::vector<CronJob> CronHandler::cronJobs;
const size_t CronHandler::MAX_CRON_JOBS = 20;

String stripQuotes(const String &str) {
    if (str.startsWith("\"") && str.endsWith("\"") && str.length() > 1) {
        return str.substring(1, str.length() - 1);
    }
    return str;
}

void CronHandler::init()
{
    debugI("* CronHandler initialized.");

    CommandHandler::registerCommand("cron", [](const String &args)
                                    {
    int spaceIndex = args.indexOf(' ');
    if (spaceIndex == -1) {
        debugI("* Invalid command. Usage: cron '<cron_expression>' '<command>'");
        return;
    }

    // Extract and trim the cron expression and command
    String cronExpression = args.substring(0, spaceIndex);
    cronExpression.trim();

    String command = args.substring(spaceIndex + 1);
    command.trim();

    // Strip quotes if present
    cronExpression = stripQuotes(cronExpression);
    command = stripQuotes(command);

    // Validate cron expression format
    auto fields = CronHandler::split(cronExpression, ' ');
    if (fields.size() != 5) {
        debugI("* Invalid cron expression format: %s", cronExpression.c_str());
        return;
    }

    CronHandler::addCronJob(command, cronExpression, true);
    debugI("* Cron job registered: '%s' to run '%s'\n", cronExpression.c_str(), command.c_str()); });
}

void CronHandler::addCronJob(const String &command, const String &cronExpression, bool repeat)
{
    if (cronJobs.size() >= MAX_CRON_JOBS)
    {
        debugI("* Error: Maximum number of cron jobs reached.");
        return;
    }
    cronJobs.emplace_back(command, cronExpression, repeat);
    debugI("* Cron job added with expression: %s to execute command: %s\n",
           cronExpression.c_str(), command.c_str());
}

void CronHandler::removeCronJob(size_t index)
{
    if (index < cronJobs.size())
    {
        cronJobs.erase(cronJobs.begin() + index);
        debugI("* Cron job at index %d removed.\n", index);
    }
    else
    {
        debugI("* Invalid cron job index.");
    }
}

void CronHandler::listCronJobs()
{
    debugI("* Listing cron jobs:");
    for (size_t i = 0; i < cronJobs.size(); ++i)
    {
        const CronJob &job = cronJobs[i];
        debugI("[%d] Expression: %s Command: %s (repeat: %d)\n",
               i, job.cronExpression.c_str(), job.command.c_str(), job.repeat);
    }
}

void CronHandler::checkCronJobs()
{
    time_t now = time(nullptr);
    struct tm *currentTime = localtime(&now);

    for (size_t i = 0; i < cronJobs.size(); ++i)
    {
        CronJob &job = cronJobs[i];
        if (parseCronExpression(job.cronExpression, currentTime))
        {
            CommandHandler::handleCommand(job.command); // Execute the command
            if (!job.repeat)
            {
                removeCronJob(i);
                --i; // Adjust index after removal
            }
        }
    }
}

bool CronHandler::matchesCronField(const String &field, int currentValue)
{
    if (field == "*")
    {
        return true; // Matches any value
    }

    if (field.indexOf(',') != -1)
    { // Handle comma-separated values
        auto values = split(field, ',');
        for (const auto &val : values)
        {
            if (val.toInt() == currentValue)
            {
                return true;
            }
        }
        return false;
    }

    if (field.indexOf('-') != -1)
    { // Handle ranges (e.g., 30-59)
        int start = field.substring(0, field.indexOf('-')).toInt();
        int end = field.substring(field.indexOf('-') + 1).toInt();
        return currentValue >= start && currentValue <= end;
    }

    // Handle single values
    return field.toInt() == currentValue;
}

bool CronHandler::parseCronExpression(const String &cronExpression, struct tm *currentTime)
{
    auto fields = split(cronExpression, ' ');

    // Debug the number of fields parsed
    debugI("* Parsing cron expression: %s", cronExpression.c_str());
    debugI("* Number of fields: %d", fields.size());

    if (fields.size() != 5)
    {
        debugI("* Invalid cron expression format.");
        return false;
    }

    return matchesCronField(fields[0], currentTime->tm_min) &&     // Minute
           matchesCronField(fields[1], currentTime->tm_hour) &&    // Hour
           matchesCronField(fields[2], currentTime->tm_mday) &&    // Day of Month
           matchesCronField(fields[3], currentTime->tm_mon + 1) && // Month
           matchesCronField(fields[4], currentTime->tm_wday);      // Day of Week
}

String trim(const String &str)
{
    int start = 0;
    while (start < str.length() && isspace(str[start]))
        start++;

    int end = str.length() - 1;
    while (end >= start && isspace(str[end]))
        end--;

    return str.substring(start, end + 1);
}

std::vector<String> CronHandler::split(const String &str, char delimiter)
{
    std::vector<String> tokens;
    int start = 0;
    int end = str.indexOf(delimiter);

    while (end != -1)
    {
        tokens.push_back(trim(str.substring(start, end))); // Trim each token
        start = end + 1;
        end = str.indexOf(delimiter, start);
    }

    tokens.push_back(trim(str.substring(start))); // Add the last token

    // Debug the split results
    debugI("* Splitting string: %s", str.c_str());
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        debugI("  Token[%d]: %s", i, tokens[i].c_str());
    }

    return tokens;
}
