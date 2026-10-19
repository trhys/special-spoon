#include "EntityMenus.h"
#include "Editor/Overlay.h"
#include "Editor/Utils/EditorSettings.h"
#include "Editor/Utils/Helpmarker.h"
#include "Editor/Blueprints/Blueprint.h"

#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"
#include "Core/Project/ProjectManager.h"
#include "ECS/Components/World/TileMapComp.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

#include <algorithm>
#include <cstdio>
#include <iterator>

namespace Spoon
{
    static bool AddingComponent = false;
    static UUID selectedID = 0;
    static bool changedSelection = false;

    // Sets the currently selected entity in the editor
    // and sets the selection rectangle to size of the
    // entities sprite/text, or to a default if neither exist
    void SelectEntity(UUID id, Editor* editor, EntityManager& manager)
    {
        selectedID = id;
        changedSelection = true;

        auto& spriteArray = manager.GetArray<SpriteComp>(SpriteComp::Name);
        auto& textArray = manager.GetArray<TextComp>(TextComp::Name);
        if (spriteArray.m_IdToIndex.count(id))
        {
            SpriteComp& sprite = manager.GetComponent<SpriteComp>(id, SpriteComp::Name);
            sf::FloatRect bounds = sprite.m_Sprite.getGlobalBounds();
            editor->m_SelectionRect.setPosition(sf::Vector2f(bounds.position.x, bounds.position.y));
            editor->m_SelectionRect.setSize(sf::Vector2f(bounds.size.x, bounds.size.y));
        }
        else if (textArray.m_IdToIndex.count(id))
        {
            TextComp& text = manager.GetComponent<TextComp>(id, TextComp::Name);
            sf::FloatRect bounds = text.m_Text.getGlobalBounds();
            editor->m_SelectionRect.setPosition(sf::Vector2f(bounds.position.x, bounds.position.y));
            editor->m_SelectionRect.setSize(sf::Vector2f(bounds.size.x, bounds.size.y));
        }
        else
            editor->m_SelectionRect.setSize(sf::Vector2f(100.f, 100.f)); // Default size
    }
}
