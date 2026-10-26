// Consumer side implementation of Sandbox.h
// Provides hooks back into the engine that can be customized

#include "Sandbox.h" // don't remove
#include "CustomSystemLoader.h" // dont remove
#include "CustomCompLoader.h" // dont remove

void Sandbox::OnProjectLoad()
{
	// your custom project load hook implementation
}

void Sandbox::OnSceneLoad() 
{
    // your custom scene load logic
}

void LoadCustomArrays(Spoon::EntityManager& manager)
{
    // load custom component arrays into the entity manage

    // EXAMPLE:
    //manager.LoadArray<AttackComp>(AttackComp::Name);
    //manager.LoadArray<HealthComp>(HealthComp::Name);
    //manager.LoadArray<HurtboxComp>(HurtboxComp::Name);
    //manager.LoadArray<RespawnComp>(RespawnComp::Name);
}

void LoadCustomActions()
{
    // load custom actions in the registry
    
    // EXAMPLE:
    //auto& actionRegistry = Spoon::ActionRegistry::Get();
	//actionRegistry.RegisterCustom("Attack");
	//actionRegistry.RegisterCustom("Die");
	//actionRegistry.RegisterCustom("Respawn");
}

void LoadCustomStates()
{
    // load custom states into the registry

    // EXAMPLE:
	//auto& stateRegistry = Spoon::StateRegistry::Get();
	//stateRegistry.RegisterCustom("Attacking");
	//stateRegistry.RegisterCustom("Dead");
	//stateRegistry.RegisterCustom("Respawning");
}

void RegisterCustomSystems()
{
    // register custom systems loaders

    //EXAMPLE:
    //Spoon::SystemRegistry::Get().RegisterLoader("Combat", &LoadCombatSystem);
    //Spoon::SystemRegistry::Get().RegisterLoader("Health", &LoadHealthSystem);
    //Spoon::SystemRegistry::Get().RegisterLoader("Respawn", &LoadRespawnSystem);
}

void RegisterCustomLoaders()
{
    // register custom component loaders

    // EXAMPLE:
    //Spoon::ComponentRegistry::Get().RegisterLoader(AttackComp::Name, &LoadAttackComponent);
    //Spoon::ComponentRegistry::Get().RegisterLoader(HealthComp::Name, &LoadHealthComponent);
    //Spoon::ComponentRegistry::Get().RegisterLoader(HurtboxComp::Name, &LoadHurtboxComponent);
    //Spoon::ComponentRegistry::Get().RegisterLoader(RespawnComp::Name, &LoadRespawnComponent);
}