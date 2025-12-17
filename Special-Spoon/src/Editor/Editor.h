#pragma once

namespace Spoon
{
    class EntityManager;
    class SceneManager;
    class SystemManager;
    struct AssetNode;
    
    class Editor
    {
    public:
        Editor() {}
        ~Editor() {}

        void Stop();
        bool Play();
        void RefreshViewport();

        void Run(EntityManager& manager, SceneManager& s_Manager, SystemManager& sys_Manager);

        void ViewEntitiesMenu(EntityManager& manager);
        void LoadSceneMenu(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager);
        void ViewResourcesMenu();
        void ViewAssets(AssetNode* node);
        
    private:
        AssetNode* workingDir = nullptr;
        bool m_Play = false;
    };

    void HelpMarker(const char* desc);
}