#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <map>
#include <functional>

class CommandHandler {
public:
    static void handleCommand(const String& command);

    static void registerCommand(const String& name, std::function<void(const String&)> handler);

private:
    static std::map<String, std::function<void(const String&)>> commandRegistry;
};

#endif // COMMAND_HANDLER_H
