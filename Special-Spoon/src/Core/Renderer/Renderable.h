#pragma once

#include "ECS/UUID.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderStates.hpp"

namespace Spoon
{
	class EntityManager;

    class IRenderable
	{
    public:
		virtual void Render(sf::RenderTarget& target, sf::RenderStates states) = 0;
		virtual void PreRender(EntityManager& manager, UUID id) = 0;
		virtual sf::Vector2f GetPosition() = 0;
	};

    struct Renderable
    {
        UUID m_ID;
        int m_Layer;
		float m_Depth;
		IRenderable* m_Component;
    };
}
