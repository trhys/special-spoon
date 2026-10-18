#pragma once

#include "Overlay.h"
#include "EditorAction.h"

#include "Tools/AnimationTool.h"
#include "Tools/TileMapTool.h"
#include "Tools/TextureRectTool.h"

#include "SFML/Graphics.hpp"

namespace Spoon
{
    struct Project;
    struct TileMapComp;
    struct Viewport;
    class Renderer;
    class EntityManager;
    class SystemManager;
    class InputSystem;
    class ProjectManager;
    class SceneManager;
    struct AssetNode;
    struct SceneData;

    // holds pointers to core services injected at construction
    struct Services
    {
        Renderer* renderer = nullptr;
        EntityManager* entityManager = nullptr;
        SystemManager* systemManager = nullptr;
        SceneManager* sceneManager = nullptr;
        ProjectManager* projectManager = nullptr;
        InputSystem* inputSystem = nullptr;
    };
    
    class Editor
    {
    public:
        Editor(Services services);

        // call this from the main loop when editor is enabled
        // it will handle all viewport-related input and rendering
        void HandleViewport(
            Viewport& viewport, 
            sf::RenderStates& states
        );

        // invokes tooling if they are active
        // ie, tile map tool
        bool HandleViewportTools(
            Viewport& viewport,
            bool viewportHovered,
            const ImVec2& imageMin,
            const ImVec2& imageMax
        );

        // pushes overlay tools to the renderer pipeline
        // see Overlay.h for more details.
        void BuildOverlay();

        // helper for ray picked entities in viewport
        // todo: maybe this should be somewhere else
        void PickEntity(UUID id);

        // Editor main loop and update functions
        void DrawMainMenuBar();

        // Project menus
        void NoProjectLoaded();
        void DrawCreateProjectPrompt();
        void DrawCreateProjectError();
        void OpenFileDialog();
        void DrawLoadProject();
        void DrawLoadProjectError();
		void DrawConfigProject();
		void DrawOverwriteProjectPrompt();
        void DrawSaveProjectError();

        // draw menus from flags
        void HandleUIState();

        // Scene controls
        void Stop();
        bool Play();
        void ResetScene();

        void Run(sf::Time tick);
        void EditTextureRect(bool& editing, sf::Texture& texture, sf::IntRect& rect);
        void EditTileMap(TileMapComp* comp);

        // config settings
        bool SetCurrentProject(Project* project);
        void SetActiveScene(SceneData* scene);
        void SetWorkingDir(AssetNode* dir) { workingDir = dir; }
        Project* GetCurrentProject() { return m_CurrentProject; }
        SceneData* GetActiveScene() { return m_ActiveScene; }
        AssetNode* GetWorkingDir() { return workingDir; }

        void Shutdown();

        bool NewScene = false;
        bool LoadScene = false;
        bool SaveScene = false;
        bool ViewSceneManifest = false;

        bool ViewEntities = true;
        bool OpenBlueprintsMenu = false;

        bool ViewResources = false;
        bool LoadResources = false;
        
        bool ViewSystemsMenu = false;

        sf::RectangleShape m_SelectionRect;

    private:
        Services m_Services;
        Project* m_CurrentProject = nullptr;
        AssetNode* workingDir = nullptr;
        SceneData* m_ActiveScene = nullptr;
        bool m_Play = false;

        EditorActionGroup   m_Actions;
        Overlay             m_Overlay;
        TextureRectTool     m_TextureRectTool;
        AnimationTool       m_AnimationTool;
        TileMapTool         m_TileMapTool;
    
    private:
        bool m_ResetScene = false;          // flag for resetting the scene
        
        // project menu flags
        bool m_ShowNoProjectLoaded = true;          // flag for showing the no project loaded message
        bool m_ShowCreateProjectPrompt = false;     // flag for showing the create project prompt
        bool m_ShowCreateProjectError = false;      // flag for showing the create project error
		bool m_ShowLoadProject = false;             // flag for loading a project
        bool m_ShowLoadProjectError = false;        // flag for showing the load project error
        bool m_ShowSaveProject = false;             // flag for saving a project
        bool m_ShowConfigProject = false;           // flag for configuring a project
		bool m_ShowOverwriteProjectPrompt = false;  // flag for showing the overwrite project prompt
		bool m_ShowSaveProjectError = false;        // flag for showing the save project error
    };
}