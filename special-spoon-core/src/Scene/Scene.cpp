#include "Scene.h"
#include "Entity.h"

namespace Spoon
{
    void Scene::Load(ResourceManager* rsm)
    {
        m_RSM = rsm;
        for (auto const& [key, value] : m_Resources)
        {
            rsm->LoadTexture(key, value);
            //SetScene();
        }
    }
}