#pragma once

extern Spoon::Application* Spoon::CreateApp();

int main(int argc, char** argv)
{
    auto sandbox = Spoon::CreateApp();
    sandbox->Spoon::Application::Run();
    delete sandbox;
}