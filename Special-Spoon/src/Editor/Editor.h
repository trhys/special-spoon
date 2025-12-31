#pragma once

#include "Menus/SystemsMenu.h"
#include "Tools/AnimationTool.h"
#include "Tools/TextureRectTool.h"

#include "SFML/System/Time.hpp"

namespace Spoon
{
    class EntityManager;
    class SystemManager;
    class SceneManager;
    struct AssetNode;
    struct SceneData;
    
    class Editor
    {
    public:
        Editor() {}
        ~Editor() {}

        void Stop();
        bool Play();

        void Run(sf::Time tick, EntityManager& manager, SceneManager& s_Manager, SystemManager& sys_Manager);

        void ViewResourcesMenu();
        void LoadResourcesMenu();
        void ViewAssets(AssetNode* node);
        void ShowTree(AssetNode* node);
        
        void EditTextureRect(SpriteComp& comp);
        
        SystemsMenu m_SystemsMenu;

        void SetActiveScene(SceneData* scene) { m_ActiveScene = scene; }

        // Menu flags
        bool NewScene = false;
        bool LoadScene = false;
        bool ViewEntities = true;
        bool ViewResources = false;
        bool LoadResources = false;

    private:
        AssetNode* workingDir = nullptr;
        SceneData* m_ActiveScene = nullptr;
        bool m_Play = false;

        TextureRectTool m_TextureRectTool;
        AnimationTool   m_AnimationTool;
    };
}