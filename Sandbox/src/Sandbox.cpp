#include "Spoon.h"
#include "TestLayer.h"

class Sandbox : public Spoon::Application
{
public:

	Sandbox();
	~Sandbox() {}

};

Sandbox::Sandbox(AppSpecifications& specs)
	: m_Specs (specs)
{
	Sandbox::PushLayer(new TestLayer())
}

Spoon::Application* Spoon::CreateApp()
{
	AppSpecifications spec;
	spec.m_WindowSize = {600, 600};
	spec.m_WindowName = "Sandbox";	
	return new Sandbox(spec);
}
