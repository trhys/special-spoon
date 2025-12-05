#pragma once

#include "Core.h"
#include "Application.h"

extern Spoon::Application* Spoon::CreateApp();

int main(int argc, char** argv)
{
    Spoon::Application* app = Spoon::CreateApp();

    SS_TRY
    
    app->Spoon::Application::Run();

    SS_CATCH("STARTUP")

    delete app;
}