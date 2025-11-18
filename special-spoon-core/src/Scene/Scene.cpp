#include "Scene.h"

namespace Spoon
{
    sf::Texture& Scene::LoadTexture(std::string id, std::filesystem::path file_path)
    {
        p_RM->LoadTexture(id, file_path);
    }

    void Scene::ShowScene() 
    { 
        m_IsActive = true; 
    }

    void Scene::HideScene() 
    { 
        m_IsActive = false; 
    }

    bool Scene::IsActive() 
    { 
        return m_IsActive; 
    }

    void Scene::GetRM(ResourceManager* rm) 
    { 
        p_RM = rm; 
    }
}