#include "SceneManager.h"

namespace Spoon
{
    void SceneManager::CacheScene(Scene scene)
    {
        auto found = m_SceneCache.find(scene.GetName());
        if(found == m_SceneCache.end())
        {
            m_SceneCache.emplace(scene.GetName(), std::move(scene));
        }
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

        m_ActiveScene = &m_SceneCache[id];
        m_ActiveScene->ShowScene();
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

        m_Overlay = &m_SceneCache[id];
        m_Overlay->ShowScene();
    }

    void SceneManager::DeactivateScene()
    {
        m_ActiveScene->HideScene();
        m_ActiveScene = nullptr;
    }

    void SceneManager::DeactivateOverlay()
    {
        m_Overlay->HideScene();
        m_Overlay = nullptr;
    }

    void SceneManager::DrawScene(sf::RenderTarget& target, sf::RenderStates states)
    {
        if(m_ActiveScene) { m_ActiveScene->draw(target, states); }
        if(m_Overlay) { m_Overlay->draw(target, states); }
    }

    void SceneManager::UpdateScene(sf::Time tick)
    {
        if(m_ActiveScene) { m_ActiveScene->Update(tick); }
    }
}