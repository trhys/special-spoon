#pragma once

namespace Spoon
{
    void Serialize(std::string SceneID, EntityManager& e_Manager, SystemManager& s_Manager)
    {
        for (auto& [id, name] : e_Manager.GetEntities())
        {
            std::vector<Component*> comps = e_Manager.GetAllComponentsOfEntity(id);
            for (auto& comp : comps)
            {
                
            }
        }
    }
}