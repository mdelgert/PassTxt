#ifdef ENABLE_DUCKYSCRIPT_HANDLER
#include "DuckyScriptHandler.h"
#include "DeviceHandler.h"
#include "LittleFS.h"
#include "KeyMappings.h"
#include <vector>

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

    if (line.isEmpty() || line.startsWith("//")) 
    {
        debugI("Skipping empty or comment line");
        return;
    }

    // Split the line into individual words (commands)
    std::vector<String> tokens;
    int start = 0, end = 0;

    while ((end = line.indexOf(' ', start)) != -1) 
    {
        String token = line.substring(start, end);
        token.trim();
        if (!token.isEmpty()) 
        {
            tokens.push_back(token);
        }
        start = end + 1;
    }

    // Add the last token (if any)
    if (start < line.length()) 
    {
        String token = line.substring(start);
        token.trim();
        if (!token.isEmpty()) 
        {
            tokens.push_back(token);
        }
    }

    // **Handling special commands first**
    if (tokens.size() == 0) return;

    String command = tokens[0];
    String args = tokens.size() > 1 ? line.substring(line.indexOf(' ') + 1) : "";

    if (CommandHandler::equalsIgnoreCase(command, "DELAY"))
    {
        int delayValue = args.toInt();
        if (delayValue > 0) 
        {
            debugI("Executing DELAY command with value: %d", delayValue);
            delay(delayValue);
        } 
        else 
        {
            debugE("Invalid DELAY value: %s", args.c_str());
        }
        return;
    }

    if (CommandHandler::equalsIgnoreCase(command, "STRING"))
    {
        if (!args.isEmpty()) 
        {
            debugI("Sending STRING: %s", args.c_str());
            DeviceHandler::sendKeys(args);
        } 
        else 
        {
            debugE("STRING command missing argument");
        }
        return;
    }

    if (CommandHandler::equalsIgnoreCase(command, "ENTER"))
    {
        debugI("Sending ENTER key");
        DeviceHandler::processKey("ENTER", true);
        delay(100);
        DeviceHandler::processKey("ENTER", false);
        return;
    }

    // **Process key combinations (hold all, then release all)**
    std::vector<String> heldKeys;
    for (String token : tokens) 
    {
        uint8_t keyCode = getKeyCode(token.c_str());
        if (keyCode != 0) 
        {
            debugI("Pressing key: %s", token.c_str());
            DeviceHandler::processKey(token, true);
            heldKeys.push_back(token);  // Track pressed keys
        } 
        else 
        {
            debugW("Unknown key: %s", token.c_str());
        }
    }

    delay(100); // Simulate realistic key press duration

    // **Release all keys before processing next line**
    for (String key : heldKeys) 
    {
        debugI("Releasing key: %s", key.c_str());
        DeviceHandler::processKey(key, false);
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