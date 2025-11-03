#include "Spoon.h"

class Sandbox : public Spoon::Application
{
public:

	Sandbox();
	~Sandbox() {}

};

Sandbox::Sandbox()
{
	PushLayer(new Layer());
}

Spoon::Application* Spoon::CreateApp()
{
	return new Sandbox();
}
