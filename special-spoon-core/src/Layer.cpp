#include "Layer.h"

namespace Spoon
{
    void Layer::LoadScene()
    {
        if(m_Scene)
        {
            m_Scene->Load(m_RSM);
        }
    }

    void Layer::DrawScene(sf::RenderTarget& target, const sf::Transform& scene_transform)
    {
        if(m_Scene)
        {
            m_Scene->draw(target, scene_transform);
        }
    }
}
