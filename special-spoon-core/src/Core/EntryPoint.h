#pragma once

#include "Core.h"
#include "Application.h"

extern Spoon::Application* Spoon::CreateApp();

int main(int argc, char** argv)
{
    Spoon::Logger::Get().Initialize("spoon_debug.log");
    
    SS_DEBUG_LOG("Starting application...")
    Spoon::Application* app = Spoon::CreateApp();

    SS_DEBUG_LOG("Running application...")
    app->Spoon::Application::Run();
    
    SS_DEBUG_LOG("Shutting down application...")
    Spoon::Logger::Get().Close();

    delete app;
}