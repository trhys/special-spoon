#include "Spoon.h"
#include "DemoLayer.h"
#include "EntryPoint.h"


//std::unordered_map<std::string, std::filesystem::path> resources =
//{
//	{"demoscene-top", "resources/TD-Scene.png" },
//	{"demoscene-side", "resources/SV-Scene.png" },
//	{"demosprite", "resources/DemoSprite.png" }
//};

class Sandbox : public Spoon::Application
{
public:

	Sandbox(const Spoon::AppSpecifications& specs);
	~Sandbox() {}

};

Sandbox::Sandbox(const Spoon::AppSpecifications& specs)
	: Spoon::Application(specs)//, resources)
{
	PushLayer(new DemoLayer());
	LoadScenes();
}

Spoon::Application* Spoon::CreateApp()
{
	AppSpecifications spec;
	spec.m_WindowSize = {600, 600};
	spec.m_WindowName = "Sandbox";	
	return new Sandbox(spec);
}
