// custom component loader functions declared here - call from init.cpp

#include "Spoon.h" // dont remove

// EXAMPLE:

//#include "Component/Combat/AttackComp.h"
//#include "Component/Combat/HealthComp.h"
//#include "Component/Combat/HurtboxComp.h"
//#include "Component/Combat/RespawnComp.h"

//void LoadAttackComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
//{
//    auto attackComp = comp.get<AttackComp>();
//    manager.MakeComponent<AttackComp>(id, AttackComp::Name);
//    auto& loaded = manager.GetComponent<AttackComp>(id, AttackComp::Name);
//    loaded = attackComp;
//}

//void LoadHealthComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
//{
//    auto healthComp = comp.get<HealthComp>();
//    manager.MakeComponent<HealthComp>(id, HealthComp::Name);
//    auto& loaded = manager.GetComponent<HealthComp>(id, HealthComp::Name);
//    loaded = healthComp;
//}

//void LoadHurtboxComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
//{
//    auto hurtComp = comp.get<HurtboxComp>();
//    manager.MakeComponent<HurtboxComp>(id, HurtboxComp::Name);
//    auto& loaded = manager.GetComponent<HurtboxComp>(id, HurtboxComp::Name);
//   loaded = hurtComp;
//}

//void LoadRespawnComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
//{
//    auto respawnComp = comp.get<RespawnComp>();
//    manager.MakeComponent<RespawnComp>(id, RespawnComp::Name);
//   auto& loaded = manager.GetComponent<RespawnComp>(id, RespawnComp::Name);
//    loaded = respawnComp;
//}

