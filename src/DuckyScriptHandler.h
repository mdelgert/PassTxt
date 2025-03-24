#pragma once

#ifdef ENABLE_DUCKYSCRIPT_HANDLER

#include "Globals.h"

class DuckyScriptHandler
{
private:
    static void executeScript(const String &filePath);
    static void processLine(const String &line);
    static void registerCommands();

public:
    static void init();
};

#else

class DuckyScriptHandler
{
public:
    static void init() {} // No-op
};

#endif // ENABLE_DUCKYSCRIPT_HANDLER
