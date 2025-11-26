#pragma once

#include "Scene.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class ResourceManager;

    class SPOON_API SceneManager
    {
    public:
        SceneManager() {}
        ~SceneManager() {}

        void Init(ResourceManager* rm);
        void CacheScene(std::string name, Scene* scene);
        void ActivateScene(std::string id);
        void ActivateOverlay(std::string id);
        void DeactivateScene();
        void DeactivateOverlay();
        void DrawScene(sf::RenderTarget& target, sf::RenderStates states);
        void UpdateScene(sf::Time tick);
        void TransitionScene();
        void SceneCleanup();

        Scene& GetSceneRef() { return *m_ActiveScene; }
        Scene* GetActiveScene() { return m_ActiveScene; }
        Scene GetOverlay() { return *m_Overlay; }

    private:
        Scene* m_ActiveScene = nullptr;
        Scene* m_Overlay = nullptr;
        ResourceManager* p_RM = nullptr;

        std::unordered_map<std::string, Scene*> m_SceneCache;
    };
}