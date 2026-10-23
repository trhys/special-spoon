#include "Core/EntryPoint.h"
#include "Spoon.h"

#include "ECS/CustomCompLoader.h"
#include "ECS/CustomSystemLoader.h"
#include "Combat/AttackSerialization.h"

class Sandbox : public Spoon::Application
{
public:

	Sandbox(const Spoon::AppSpecifications& specs);

	void OnProjectLoad() override {
		init::InitializeAttackRegistry();
	}
	void OnSceneLoad() override {}
};

// load the engine's entity manager with custom
// component arrays
void LoadCustomArrays(Spoon::EntityManager& manager)
{
	manager.LoadArray<AttackComp>(AttackComp::Name);
	manager.LoadArray<HealthComp>(HealthComp::Name);
	manager.LoadArray<HurtboxComp>(HurtboxComp::Name);
	manager.LoadArray<RespawnComp>(RespawnComp::Name);
}

// register custom actions
void LoadCustomActions()
{
	auto& actionRegistry = Spoon::ActionRegistry::Get();
	actionRegistry.RegisterCustom("Attack");
	actionRegistry.RegisterCustom("Die");
	actionRegistry.RegisterCustom("Respawn");
}

// register custom states
void LoadCustomStates()
{
	auto& stateRegistry = Spoon::StateRegistry::Get();
	stateRegistry.RegisterCustom("Attacking");
	stateRegistry.RegisterCustom("Dead");
	stateRegistry.RegisterCustom("Respawning");
}

// Sandbox application constructor and initialization
// Initializes the Sandbox application app specifications
// and sets up the custom components and systems
Sandbox::Sandbox(const Spoon::AppSpecifications& specs)
	: Spoon::Application(specs)
{
	SS_DEBUG_LOG("Loading custom components")
	RegisterCustomLoaders();
	SS_DEBUG_LOG("Loading custom arrays")
	LoadCustomArrays(GetEntityManager());
	SS_DEBUG_LOG("Custom loaders initialized")
	SS_DEBUG_LOG("Loading custom actions")
	LoadCustomActions();
	SS_DEBUG_LOG("Custom actions loaded")
	SS_DEBUG_LOG("Loading custom states")
	LoadCustomStates();
	SS_DEBUG_LOG("Custom states loaded")
	SS_DEBUG_LOG("Loading custom systems")
	RegisterCustomSystems();
	SS_DEBUG_LOG("Custom systems loaded")
}

// extern function to create the application instance
// see Application.h
Spoon::Application* Spoon::CreateApp()
{
	AppSpecifications spec;
	spec.windowName = "Sandbox";
	return new Sandbox(spec);
}
