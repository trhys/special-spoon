#pragma once

#include "Scene.h"

#include "SFML/Graphics.hpp"
#include <functional>

namespace Spoon
{
    //class Application;

    class SceneManager
    {
    public:
        SceneManager() {}
        ~SceneManager() {}

        //void Init(Application* app) { p_App = app; }

        void CacheScene(Scene scene);
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

		}

    private:
        //Application* p_App = nullptr;
        Scene* m_ActiveScene = nullptr;
        Scene* m_Overlay = nullptr;

        std::unordered_map<std::string, Scene> m_SceneCache;
        //std::vector<std::function<void()>> m_Creationbuffer;
    };
}