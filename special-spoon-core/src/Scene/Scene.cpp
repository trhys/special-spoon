#include "Scene/Scene.h"
#include "Entity.h"

namespace Spoon
{
    void Scene::Load(ResourceManager& rsm)
    {
        m_RSM = rsm;
        for (auto it : m_Resources.begin(); it != m_Resources.end(); it++)
        {
            rsm->LoadTexture(it->first, it->second);
            SetScene();
        }
    }
}