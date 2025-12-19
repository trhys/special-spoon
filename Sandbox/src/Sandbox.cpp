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
	manager.LoadArray<PatrolComp>();
	manager.LoadArray<MovementComp>();
	manager.LoadArray<PlayerComp>();
}

Sandbox::Sandbox(const Spoon::AppSpecifications& specs)
	: Spoon::Application(specs)
{
	SS_DEBUG_LOG("Loading custom components")
	RegisterCustomLoaders();
	SS_DEBUG_LOG("Loading custom arrays")
	LoadCustomArrays(GetEntityManager());
	SS_DEBUG_LOG("Custom loaders initialized")

	SS_DEBUG_LOG("Loading custom systems")
	RegisterCustomSystems();
	SS_DEBUG_LOG("Custom systems loaded")
}

Spoon::Application* Spoon::CreateApp()
{
	AppSpecifications spec;
	spec.windowName = "Sandbox";
	return new Sandbox(spec);
}

