#pragma once

extern Spoon::Application* Spoon::CreateApp();

int main(int argc, char** argv)
{
    auto app = Spoon::CreateApp();
    app->Run();
    delete app;
}