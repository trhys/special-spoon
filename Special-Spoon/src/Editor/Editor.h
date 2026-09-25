#pragma once

#include "Overlay.h"

#include "Tools/AnimationTool.h"
#include "Tools/TileMapTool.h"
#include "Tools/TextureRectTool.h"

#include "SFML/System/Time.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

namespace Spoon
{
    struct Project;
    struct TileMapComp;
    struct Viewport;
    class EntityManager;
    class SystemManager;
    class SceneManager;
    struct AssetNode;
    struct SceneData;
    
    class Editor
    {
    public:
        // Scene controls
        void Stop();
        bool Play();
        void ResetScene(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager);

        void Run(sf::Time tick, EntityManager& manager, SceneManager& s_Manager, SystemManager& sys_Manager);       
        void EditTextureRect(SpriteComp& comp);
        void EditTileMap(UUID id);
        void PickEntity(UUID id, EntityManager& e_Manager);
        bool HandleViewportTools(
            Viewport& viewport,
            bool viewportHovered,
            const ImVec2& imageMin,
            const ImVec2& imageMax
        );

        void BuildOverlay();

        void SetCurrentProject(Project* project) { m_CurrentProject = project; }
        void SetActiveScene(SceneData* scene);
        void SetWorkingDir(AssetNode* dir) { workingDir = dir; }

        Project* GetCurrentProject() { return m_CurrentProject; }
        SceneData* GetActiveScene() { return m_ActiveScene; }
        AssetNode* GetWorkingDir() { return workingDir; }

        void Shutdown();

        // Menu flags
        bool NewProject = false;
        bool OpenProject = false;
        bool SaveProject = false;
        bool ProjectProperties = false;

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
        Project* m_CurrentProject = nullptr;
        AssetNode* workingDir = nullptr;
        SceneData* m_ActiveScene = nullptr;
        bool m_Play = false;

        Overlay         m_Overlay;
        TextureRectTool m_TextureRectTool;
        AnimationTool   m_AnimationTool;
        TileMapTool     m_TileMapTool;
    };
}