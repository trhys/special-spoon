

extern Spoon::Application* Spoon::CreateApp();

int main(int argc, char** argv)
{
    auto sandbox = Spoon::CreateApp();
    sandbox->Run();
    delete sandbox;
}