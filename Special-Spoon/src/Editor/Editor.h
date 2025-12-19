#pragma once

namespace Spoon
{
    class EntityManager;
    class SceneManager;
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

        void Run(EntityManager& manager, SceneManager& s_Manager, SystemManager& sys_Manager);

        void ViewEntitiesMenu(EntityManager& manager);
        void AddComponentMenu(UUID id);

        void NewSceneMenu(SceneManager& s_Manager);
        void LoadSceneMenu(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager);

        void ViewResourcesMenu();
        void ViewAssets(AssetNode* node);
        
    private:
        AssetNode* workingDir = nullptr;
        bool m_Play = false;
    };

    void HelpMarker(const char* desc);
}