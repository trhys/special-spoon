#include <Spoon.h>

class Sandbox : public Spoon::Application
{
public:

	Sandbox() {};
	~Sandbox() {};

}

Spoon::Application* CreateApp()
{
	return new Sandbox();
}