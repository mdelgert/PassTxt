#ifdef ENABLE_DUCKYSCRIPT_HANDLER
#include "DuckyScriptHandler.h"
#include "DeviceHandler.h"
#include "LittleFS.h"
#include "KeyMappings.h"

void DuckyScriptHandler::init()
{
    registerCommands();
    debugI("DuckyScriptHandler initialized");
}

void DuckyScriptHandler::executeScript(const String &filePath)
{
    debugI("Attempting to execute DuckyScript from file: %s", filePath.c_str());
    File file = LittleFS.open(filePath, "r");
    if (!file)
    {
        debugE("Failed to open DuckyScript file: %s", filePath.c_str());
        return;
    }

    debugI("Executing DuckyScript from: %s", filePath.c_str());
    while (file.available())
    {
        String line = file.readStringUntil('\n');
        line.trim();
        if (!line.isEmpty() && !line.startsWith("//")) // Ignore empty lines and comments
        {
            debugI("Read line: %s", line.c_str());
            processLine(line);
        }
        else if (line.startsWith("//"))
        {
            debugI("Skipping comment line: %s", line.c_str());
        }
        else
        {
            debugI("Skipping empty line");
        }
    }

    file.close();
    debugI("DuckyScript execution completed");
}

void DuckyScriptHandler::processLine(const String &line)
{
    debugI("Processing DuckyScript line: %s", line.c_str());

    int start = 0, end = 0;
    while ((end = line.indexOf(' ', start)) != -1)
    {
        String commandSegment = line.substring(start, end);
        commandSegment.trim(); // Trim the segment before processing
        executeCommand(commandSegment);
        start = end + 1;
    }

    // Process the last command segment
    if (start < line.length())
    {
        String remainingSegment = line.substring(start);
        remainingSegment.trim(); // Trim the remaining part of the line before processing
        executeCommand(remainingSegment);
    }
}

void DuckyScriptHandler::executeCommand(const String &commandSegment)
{
    debugI("Executing command segment: %s", commandSegment.c_str());

    String cmd, args;
    CommandHandler::parseCommand(commandSegment, cmd, args);

    if (CommandHandler::equalsIgnoreCase(cmd, "DELAY"))
    {
        debugI("Executing DELAY command with value: %d", args.toInt());
        delay(args.toInt());
    }
    else if (CommandHandler::equalsIgnoreCase(cmd, "STRING"))
    {
        debugI("Sending STRING '%s'", args.c_str());
        DeviceHandler::sendKeys2(args);
    }
    else if (CommandHandler::equalsIgnoreCase(cmd, "DOWN"))
    {
        debugI("Processing DOWN command with key: %s", args.c_str());
        uint8_t keyCode = getKeyCode(args.c_str());
        if (keyCode != 0)
        {
            DeviceHandler::processKey(args, true); // Press key
        }
        else
        {
            debugE("Invalid keycode for DOWN command: %s", args.c_str());
        }
    }
    else if (CommandHandler::equalsIgnoreCase(cmd, "UP"))
    {
        debugI("Processing UP command with key: %s", args.c_str());
        uint8_t keyCode = getKeyCode(args.c_str());
        if (keyCode != 0)
        {
            DeviceHandler::processKey(args, false); // Release key
        }
        else
        {
            debugE("Invalid keycode for UP command: %s", args.c_str());
        }
    }
    else if (CommandHandler::equalsIgnoreCase(cmd, "PRINT"))
    {
        debugI("Executing PRINT command with message: %s", args.c_str());
    }
    else
    {
        uint8_t keyCode = getKeyCode(cmd.c_str());
        if (keyCode != 0)
        {
            debugI("Executing key press for keycode: %d", keyCode);
            DeviceHandler::processKey(cmd, true);  // Press key
            delay(100);                            // Small delay to simulate key press
            DeviceHandler::processKey(cmd, false); // Release key
        }
        else
        {
            uint8_t mouseCode = getMouseButtonCode(cmd.c_str());
            if (mouseCode != 0)
            {
                debugI("Executing mouse button action with code: %d", mouseCode);
                DeviceHandler::sendMouseMovement(mouseCode, 0);
            }
            else
            {
                debugW("Unknown DuckyScript command: %s", commandSegment.c_str());
            }
        }
    }
}

void DuckyScriptHandler::registerCommands()
{
    CommandHandler::registerCommand("DUCKY", [](const String &command)
                                    {
        debugI("Processing DUCKY command: %s", command.c_str());

        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);
        if (CommandHandler::equalsIgnoreCase(cmd, "FILE")) {
            if (args.isEmpty()) {
                debugE("Missing file path for DUCKY FILE command.");
                return;
            }
            executeScript(args);
        } else if (CommandHandler::equalsIgnoreCase(cmd, "LINE")) {
            processLine(args.c_str());
        } else {
            debugW("Unknown DUCKY subcommand: %s", cmd.c_str());
        } }, "Usage: DUCKY FILE <file_path>"
             "  FILE <file_path> - Executes DuckyScript from the specified file\n"
             "  LINE <line> - Processes a single line of DuckyScript"
        );
}

#endif // ENABLE_DUCKYSCRIPT_HANDLER