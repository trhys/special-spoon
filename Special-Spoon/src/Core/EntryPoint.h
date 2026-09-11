#pragma once

#include "Core.h"
#include "Application.h"
#include "Utils/Macros.h"
#include <iostream>
extern Spoon::Application* Spoon::CreateApp();

int main(int argc, char** argv)
{
    if (argc > 0)
    {
        std::filesystem::path exeDir = std::filesystem::absolute(argv[0]);
        std::filesystem::path exePath = exeDir.parent_path();
        std::filesystem::current_path(exePath);
    }
    std::cout << std::filesystem::current_path() << std::endl;
    Spoon::Logger::Get().Initialize("spoon_debug.log");
    
    SS_DEBUG_LOG("Starting application...")
    Spoon::Application* app = Spoon::CreateApp();

    SS_DEBUG_LOG("Running application...")
    app->Spoon::Application::Run();
    
    delete app;
    app = nullptr;

    SS_DEBUG_LOG("Shutting down application...")
    Spoon::Logger::Get().Close();

    
}