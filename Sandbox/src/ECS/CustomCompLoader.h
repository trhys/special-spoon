#include "Spoon.h"

#include "Component/Combat/AttackComp.h"

void LoadAttackComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
{
    auto attackComp = comp.get<AttackComp>();
    manager.MakeComponent<AttackComp>(id, AttackComp::Name);
    auto& loaded = manager.GetComponent<AttackComp>(id, AttackComp::Name);
    loaded = attackComp;
}

void RegisterCustomLoaders()
{
    Spoon::ComponentRegistry::Get().RegisterLoader(AttackComp::Name, &LoadAttackComponent);
}
