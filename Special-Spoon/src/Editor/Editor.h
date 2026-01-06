#pragma once

#include "Tools/AnimationTool.h"
#include "Tools/TextureRectTool.h"

#include "SFML/System/Time.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

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
        void Stop();
        bool Play();
        void Run(sf::Time tick, EntityManager& manager, SceneManager& s_Manager, SystemManager& sys_Manager);       
        void EditTextureRect(SpriteComp& comp);
        void PickEntity(UUID id, EntityManager& e_Manager);

        void SetActiveScene(SceneData* scene) { m_ActiveScene = scene; }
        void SetWorkingDir(AssetNode* dir) { workingDir = dir; }

        SceneData* GetActiveScene() { return m_ActiveScene; }
        AssetNode* GetWorkingDir() { return workingDir; }

        void Shutdown();

        // Menu flags
        bool NewScene = false;
        bool LoadScene = false;
        bool SaveScene = false;
        bool ViewSceneManifest = false;
        bool ViewEntities = true;
        bool ViewResources = false;
        bool LoadResources = false;
        bool ViewSystemsMenu = false;

        sf::RectangleShape m_SelectionRect;

    private:
        AssetNode* workingDir = nullptr;
        SceneData* m_ActiveScene = nullptr;
        bool m_Play = false;

        TextureRectTool m_TextureRectTool;
        AnimationTool   m_AnimationTool;
    };
}