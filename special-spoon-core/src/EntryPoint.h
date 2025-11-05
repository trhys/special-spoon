#pragma once

#include "Core.h"
#include "Application.h"

extern Spoon::Application* Spoon::CreateApp();

int main(int argc, char** argv)
{
    SS_TRY

    auto app = Spoon::CreateApp();
    app->Run();

    SS_CATCH(SS_CPROF_STARTUP)

    delete app;
}