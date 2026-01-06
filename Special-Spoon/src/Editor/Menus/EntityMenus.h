#pragma once

namespace Spoon
{
    class Editor;
    class EntityManager;
    struct UUID;

    void ViewEntitiesMenu(EntityManager& e_Manager);
    void AddComponentMenu(UUID& id, EntityManager& manager);
    void SelectEntity(UUID id, Editor* editor, EntityManager& manager);
}