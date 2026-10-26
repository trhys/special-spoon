#include "Core/EntryPoint.h"
#include "Spoon.h"

class Sandbox : public Spoon::Application
{
public:

	Sandbox(const Spoon::AppSpecifications& specs);

	// hooks customizable in the client module
	void OnProjectLoad() override;
	void OnSceneLoad() override;
};

// custom registration hooks
void LoadCustomArrays(Spoon::EntityManager& manager);
void LoadCustomActions();
void LoadCustomStates();
void RegisterCustomLoaders();
void RegisterCustomSystems();

Sandbox::Sandbox(const Spoon::AppSpecifications& specs)
	: Spoon::Application(specs)
{
	RegisterCustomLoaders();
	LoadCustomArrays(GetEntityManager());
	LoadCustomActions();
	LoadCustomStates();
	RegisterCustomSystems();
}

// extern function to create the application instance
// see Application.h
Spoon::Application* Spoon::CreateApp()
{
	AppSpecifications spec;
	spec.windowName = "Sandbox";
	return new Sandbox(spec);
}
