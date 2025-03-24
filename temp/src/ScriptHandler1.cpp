#ifdef ENABLE_SCRIPT_HANDLER

#include "ScriptHandler.h"

static unsigned long defaultDelay = 0;         // Default delay between commands
static std::vector<String> repeatBuffer;      // Buffer to store lines for REPEAT
static unsigned int repeatCount = 0;          // Number of times to repeat

void ScriptHandler::loop() {}

void ScriptHandler::init()
{
    if (!LittleFS.begin()) {
        debugE("* LittleFS initialization failed!");
        return;
    }

    registerCommands();
    debugI("ScriptHandler initialized.");
}

void ScriptHandler::handleScriptFile(const String &args)
{
    if (args.isEmpty()) {
        debugW("SCRIPT FILE requires a file path. Usage: SCRIPT FILE <path>");
        return;
    }

    if (!LittleFS.exists(args)) {
        debugW("Script file not found: %s", args.c_str());
        return;
    }

    File scriptFile = LittleFS.open(args, "r");
    if (!scriptFile) {
        debugE("Failed to open script file: %s", args.c_str());
        return;
    }

    debugI("Executing script file: %s", args.c_str());

    while (scriptFile.available()) {
        String line = scriptFile.readStringUntil('\n');
        line.trim(); // Remove any leading/trailing whitespace or newlines

        if (line.isEmpty() || line.startsWith("REM")) { // Ignore empty lines and comments
            continue;
        }

        // Parse the command and handle special script commands
        if (!handleSpecialScriptCommand(line)) {
            debugD("Executing line: %s", line.c_str());
            CommandHandler::handleCommand(line);

            // Apply the default delay after each command
            if (defaultDelay > 0) {
                debugD("Applying default delay: %lu ms", defaultDelay);
                vTaskDelay(pdMS_TO_TICKS(defaultDelay));
            }
        }
    }

    scriptFile.close();
    debugI("Finished executing script file: %s", args.c_str());
}

bool ScriptHandler::handleSpecialScriptCommand(const String &line)
{
    String command, args;
    CommandHandler::parseCommand(line, command, args);

    if (CommandHandler::equalsIgnoreCase(command, "DEFAULTDELAY")) {
        defaultDelay = args.toInt();
        debugI("Set default delay to %lu ms", defaultDelay);
        return true; // Special command handled
    }

    if (CommandHandler::equalsIgnoreCase(command, "DELAY")) {
        unsigned long delayTime = args.toInt();
        debugI("Applying delay of %lu ms", delayTime);
        vTaskDelay(pdMS_TO_TICKS(delayTime)); // Non-blocking delay
        return true; // Special command handled
    }

    if (CommandHandler::equalsIgnoreCase(command, "REPEAT")) {
        repeatCount = args.toInt();
        debugI("Repeating buffered commands %u times", repeatCount);

        for (unsigned int i = 0; i < repeatCount; i++) {
            for (const String &repeatLine : repeatBuffer) {
                debugD("Replaying line: %s", repeatLine.c_str());
                CommandHandler::handleCommand(repeatLine);

                // Apply the default delay after each command in the repeat buffer
                if (defaultDelay > 0) {
                    vTaskDelay(pdMS_TO_TICKS(defaultDelay)); // Non-blocking delay
                }
            }
        }

        repeatBuffer.clear(); // Clear the buffer after execution
        return true; // Special command handled
    }

    // If not a special command, buffer the line for REPEAT
    if (!repeatCount && repeatBuffer.size() < 100) { // Prevent excessive buffering
        repeatBuffer.push_back(line);
    }

    return false; // Not a special command
}

void ScriptHandler::registerCommands()
{
    CommandHandler::registerCommand("SCRIPT", [](const String &command) {
        String subCommand, args;
        CommandHandler::parseCommand(command, subCommand, args);

        if (CommandHandler::equalsIgnoreCase(subCommand, "FILE")) {
            handleScriptFile(args);
        } else {
            debugW("Unknown SCRIPT subcommand: %s", subCommand.c_str());
        }
    },
    "Handles SCRIPT commands. Usage: SCRIPT <subcommand> [args]\n"
    "  Subcommands:\n"
    "  file <path> - Executes commands from the specified script file.");
}

#endif // ENABLE_SCRIPT_HANDLER
