#include "Spoon.h"

#include "ECS/CustomCompLoader.h"
#include "ECS/CustomSystemLoader.h"

class Sandbox : public Spoon::Application
{
public:

	Sandbox(const Spoon::AppSpecifications& specs);
	~Sandbox() {}
};

void LoadCustomArrays(Spoon::EntityManager& manager)
{
	SS_DEBUG_LOG("Loading Patrol Component Array")
	manager.LoadArray<PatrolComp>();
}

Sandbox::Sandbox(const Spoon::AppSpecifications& specs)
	: Spoon::Application(specs)
{
	SS_DEBUG_LOG("Loading custom components")
	RegisterCustomLoaders();
	LoadCustomArrays(GetEntityManager());
	SS_DEBUG_LOG("Custom loaders initialized")

	RegisterCustomSystems();

	GetSceneManager().LoadManifest("assets/scene/scene_manifest.json");
	GetSceneManager().LoadScene("MainMenu", GetEntityManager(), GetSystemManager());

}

Spoon::Application* Spoon::CreateApp()
{
	AppSpecifications spec;
	spec.m_WindowSize = {1080, 1080};
	spec.m_WindowName = "Sandbox";
	return new Sandbox(spec);
}

