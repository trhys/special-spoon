#include "SceneManager.h"
#include "ResourceManager.h"

namespace Spoon
{
    void SceneManager::Init(ResourceManager* rm)
    {
        p_RM = rm;
    }

    void SceneManager::CacheScene(std::string name, Scene* scene)
    {
        auto found = m_SceneCache.find(name);
        if(found == m_SceneCache.end()) // Initialize new scene
        {
            scene->Init(p_RM);
            m_SceneCache.emplace(name, std::move(scene));
        }
        else // Replace scene with new
        {
            delete m_SceneCache[name];
            scene->Init(p_RM);
            m_SceneCache[name] = std::move(scene);
        }
        scene->OnCache();
    }

    void SceneManager::ActivateScene(std::string id)
    {
        if(m_ActiveScene)
        {
            DeactivateScene();
        }
        auto found = m_SceneCache.find(id);
        if(found == m_SceneCache.end())
        {
            throw std::runtime_error("The scene you are trying to activate does not exist! : " + id);
        }

        m_ActiveScene = m_SceneCache[id];
        m_ActiveScene->OnStart();
    }

    void SceneManager::ActivateOverlay(std::string id)
    {
        if(m_Overlay)
        {
            DeactivateOverlay();
        }
        auto found = m_SceneCache.find(id);
        if(found == m_SceneCache.end())
        {
            throw std::runtime_error("The overlay you are trying to activate does not exist! : " + id);
        }

        m_Overlay = m_SceneCache[id];
        m_Overlay->OnStart();
    }

    void SceneManager::DeactivateScene()
    {
        m_ActiveScene->OnEnd();
        m_ActiveScene = nullptr;
    }

    void SceneManager::DeactivateOverlay()
    {
        m_Overlay->OnEnd();
        m_Overlay = nullptr;
    }

    // void SceneManager::UpdateScene(sf::Time tick)
    // {
    //     if(m_ActiveScene) { m_ActiveScene->Update(tick); }
    //     if(m_Overlay) { m_Overlay->Update(tick); }
    // }

    void SceneManager::TransitionScene()
    {
        if(m_ActiveScene)
        {
            m_ActiveScene->OnTransition();
        }
    }

    // void SceneManager::SceneCleanup()
    // {
    //     if(m_ActiveScene) { m_ActiveScene->Cleanup(); }
    //     if(m_Overlay) { m_Overlay->Cleanup(); }
    // }
}