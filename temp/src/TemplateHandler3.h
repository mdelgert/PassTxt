#pragma once

#ifdef ENABLE_TEMPLATE_HANDLER

#include "Globals.h"

class TemplateHandler
{
private:
    static void examplePrivate();

public:
    static void init();
    static void loop();
    static void examplePublic();
};

#else

class TemplateHandler { 
public: // No-op implementation of TemplateHandler
    static void init() {} // No-op
    static void loop() {} // No-op
    static void examplePublic() {} // No-op;
};

#endif // ENABLE_TEMPLATE_HANDLER