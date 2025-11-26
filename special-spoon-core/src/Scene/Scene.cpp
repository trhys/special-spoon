#include "Scene.h"
#include "ResourceManager.h"

namespace Spoon
{
    sf::Texture& Scene::LoadTexture(std::string id, std::filesystem::path file_path)
    {
        return p_RM->LoadTexture(id, file_path);
    }

    sf::Font& Scene::LoadFont(std::string id, std::filesystem::path file_path)
    {
        return p_RM->LoadFont(id, file_path);
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

    void Scene::Init(ResourceManager* rm) 
    { 
        p_RM = rm; 
    }

    std::vector<Node*>& Scene::GetCollidablesGraph()
    {
        m_CollidablesGraph.clear();
        for(auto& child : GetChildren())
        {
            SendNodes(m_CollidablesGraph);
        }
        return m_CollidablesGraph;
    }
}