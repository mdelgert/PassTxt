#ifdef ENABLE_TEMPLATE_HANDLER

#include "TemplateHandler.h"

static NonBlockingTimer myTimer(1000);

void TemplateHandler::init()
{
    debugI("TemplateHandler initialized");
}

void TemplateHandler::loop()
{
    if (myTimer.isReady())
    {
        debugI("TemplateHandler loop");
    }
}

void TemplateHandler::examplePublic()
{
    // Example of debug levels
    debugV("* This is a message of debug level VERBOSE");
    debugD("* This is a message of debug level DEBUG");
    debugI("* This is a message of debug level INFO");
    debugW("* This is a message of debug level WARNING");
    debugE("* This is a message of debug level ERROR");
}

void TemplateHandler::examplePrivate()
{
    debugI("TemplateHandler examplePrivate");
}

#endif // ENABLE_TEMPLATE_HANDLER






    /*
    
    debugI("* Initializing CommandHandler...");
    // Register a "hello" command
    CommandHandler::registerCommand("hello", [](const String &args)
                                    {
        debugI("* Executing 'hello' command with args: %s", args.c_str());
        if (args.isEmpty()) {
            debugI("Hello, world!");
        } else {
            debugI("Hello, %s!", args.c_str());
        } }, "Greets the user. Usage: hello [name]");

    // Register a "help" command
    CommandHandler::registerCommand("help", [](const String &)
                                    { CommandHandler::listCommands(); }, "Lists all available commands.");
    debugI("* CommandHandler initialized.");
    
    */