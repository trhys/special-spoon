#include "Spoon.h"

#include "ECS/CustomCompLoader.h"
#include "ECS/CustomSystemLoader.h"

class Sandbox : public Spoon::Application
{
public:

	Sandbox(const Spoon::AppSpecifications& specs);
	~Sandbox() {}
};

Sandbox::Sandbox(const Spoon::AppSpecifications& specs)
	: Spoon::Application(specs)
{
	RegisterCustomLoaders();
	RegisterCustomSystems();

	GetSceneManager().LoadManifest("assets/scene/scene_manifest.json");
	GetSceneManager().LoadScene("MainMenu", GetEntityManager(), GetSystemManager());
}

Spoon::Application* Spoon::CreateApp()
{
	AppSpecifications spec;
	spec.m_WindowSize = {1080, 1080};
	spec.m_WindowName = "Sandbox";
	spec.PhysicsEnabled = true;
	return new Sandbox(spec);
}
