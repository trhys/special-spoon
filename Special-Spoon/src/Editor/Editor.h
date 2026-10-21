#pragma once

#include "Services.h"

#include "Tools/AnimationTool.h"
#include "Tools/TileMapTool.h"
#include "Tools/TextureRectTool.h"

#include "SFML/Graphics.hpp"

namespace Spoon
{
    struct Project;
    struct TileMapComp;
    struct AssetNode;
    struct SceneData;
    struct Viewport;
    
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

        // helper for ray picked entities in viewport
        // todo: maybe this should be somewhere else
        void PickEntity(UUID id);

        // main ui dashboard
        void DrawMainMenuBar();
        void DrawInspector(sf::Time& tick);
        
        // util
        void DrawMetrics(sf::Time& tick); // called from inspector

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

        // entity menus/widgets
        void DrawEntityList(); // called from inspector
        void DrawCreateEntity();
        void DrawComponentList(); // called from inspector
        void DrawAddComponent();
        void DrawDeleteComponent();
        void DrawBlueprintsMenu();

        // scene menus
        void DrawNewScene();
        void DrawSaveScene();
        void DrawLoadScene();
        void DrawDeleteScenes();

        // system menus
        void DrawSystemsMenu();

        // resource menus
        void DrawResourcesMenu();

        void DrawShutdownPrompt();

        // tools
        void EditTextureRect(bool& editing, sf::Texture& texture, sf::IntRect& rect);
        void EditTileMap(TileMapComp* comp);

        // config settings
        bool SetCurrentProject(Project* project);
        void SetActiveScene(SceneData* scene);
        void SetWorkingDir(AssetNode* dir) { workingDir = dir; }
        Project* GetCurrentProject() { return m_CurrentProject; }
        SceneData* GetActiveScene() { return m_ActiveScene; }
        AssetNode* GetWorkingDir() { return workingDir; }

        // Scene controls
        void Stop();
        bool Play();
        void ResetScene();

        // lifecycle
        void Run(sf::Time tick);
        void RequestQuit();
        void Shutdown();
        
        sf::RectangleShape m_SelectionRect;

    private:
        Services m_Services;
        Project* m_CurrentProject = nullptr;
        AssetNode* workingDir = nullptr;
        SceneData* m_ActiveScene = nullptr;
        bool m_Play = false;

        TextureRectTool     m_TextureRectTool;
        AnimationTool       m_AnimationTool;
        TileMapTool         m_TileMapTool;

        UUID m_SelectedEntity = -1;
        Component* m_SelectedComponent = nullptr;
    
    private:
        bool m_ResetScene = false;          // flag for resetting the scene
        
        bool m_ShowQuitPrompt = false;                  // flag for showing the quit prompt

        // project menu flags
        bool m_ShowNoProjectLoaded = true;          // flag for showing the no project loaded message
        bool m_ShowCreateProjectPrompt = false;     // flag for showing the create project prompt
        bool m_ShowCreateProjectError = false;      // flag for showing the create project error
		bool m_ShowLoadProject = false;             // flag for loading a project
        bool m_ShowLoadProjectError = false;        // flag for showing the load project error
        bool m_ShowConfigProject = false;           // flag for configuring a project
		bool m_ShowOverwriteProjectPrompt = false;  // flag for showing the overwrite project prompt
		bool m_ShowSaveProjectError = false;        // flag for showing the save project error

        // entity menu flags
        bool m_ShowEntityList = true;
        bool m_ShowCreateEntity = false;
        bool m_ShowComponentList = true;
        bool m_ShowAddComponent = false;
        bool m_ShowDeleteComponent = false;
        bool m_ShowBlueprintsMenu = false;

        // scene menu flags
        bool m_ShowNewScene = false;
        bool m_ShowLoadScene = false;
        bool m_ShowSaveScene = false;
        bool m_ShowDeleteScene = false;

        // system menu flags
        bool m_ShowSystemsMenu = false;

        // resource menu flags
        bool m_ShowResourcesMenu = false;

        // utility widget flags
        bool m_ShowMetrics = false;

    private:
        // config flags
        bool m_SkipOverwriteSavePrompt = false;
        bool m_SkipDeleteComponentPrompt = false;
    };
}