#include "Spoon.h"
#include "DemoLayer.h"
#include "DemoMenu.h"
#include "EntryPoint.h"

class Sandbox : public Spoon::Application
{
public:

	Sandbox(const Spoon::AppSpecifications& specs);
	~Sandbox() {}
};

Sandbox::Sandbox(const Spoon::AppSpecifications& specs)
	: Spoon::Application(specs)
{
	PushLayer(new DemoMenu());
}

Spoon::Application* Spoon::CreateApp()
{
	AppSpecifications spec;
	spec.m_WindowSize = {1080, 1080};
	spec.m_WindowName = "Sandbox";
	spec.PhysicsEnabled = true;
	return new Sandbox(spec);
}
