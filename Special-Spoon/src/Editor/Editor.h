#pragma once

#include "SystemsMenu.h"
#include "Core/SceneManager.h"
#include "SFML/System/Time.hpp"
#include "TextureRectTool.h"
#include "AnimationTool.h"

namespace Spoon
{
    class EntityManager;
    class SystemManager;
    struct AssetNode;
    struct UUID;
    
    class Editor
    {
    public:
        Editor() {}
        ~Editor() {}

        void Stop();
        bool Play();

        void Run(sf::Time tick, EntityManager& manager, SceneManager& s_Manager, SystemManager& sys_Manager);

        void ViewEntitiesMenu(EntityManager& manager);
        void AddComponentMenu(UUID id, EntityManager& manager);

        void NewSceneMenu(SceneManager& s_Manager);
        void LoadSceneMenu(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager);

        void ViewResourcesMenu();
        void LoadResourcesMenu();
        void ViewAssets(AssetNode* node);
        void ShowTree(AssetNode* node);
        
        void EditTextureRect(SpriteComp& comp);
        
        SystemsMenu m_SystemsMenu;

    private:
        AssetNode* workingDir = nullptr;
        SceneData* m_ActiveScene = nullptr;
        bool m_Play = false;

        TextureRectTool m_TextureRectTool;
        AnimationTool   m_AnimationTool;
    };

    void HelpMarker(const char* desc);
}