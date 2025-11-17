#pragma once

#include "Scene.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Scene;

    class SceneManager
    {
    public:
        SceneManager() {}
        ~SceneManager() {}

        void CacheScene(std::string name, sf::Vector2f size);
        void ActivateScene(std::string id);
        void ActivateOverlay(std::string id);
        void DeactivateScene();
        void DeactivateOverlay();
        void DrawScene(sf::RenderTarget& target, sf::RenderStates states);
        void UpdateScene(sf::Time tick);

        Scene GetActiveScene() { return *m_ActiveScene; }
        Scene GetOverlay() { return *m_Overlay; }

        template <typename T>
		void CreateSceneEntity(T entity)
		{
            m_ActiveScene->AddChild(entity);
		}

        template <typename T>
		void CreateOverlayEntity(std::string name, std::filesystem::path file_path)
		{
            // TODO
		}

    private:
        Scene* m_ActiveScene = nullptr;
        Scene* m_Overlay = nullptr;

        std::unordered_map<std::string, Scene> m_SceneCache;
    };
}