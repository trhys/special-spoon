#pragma once

namespace Spoon
{
    class Editor;
    class EntityManager;
    struct UUID;

    void SelectEntity(UUID id, Editor* editor, EntityManager& manager);
}
