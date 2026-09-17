#pragma once

#include "Core/EntityManager/EntityManager.h
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderStates.hpp"

namespace Spoon
{
    class IRenderable
	{
    public:
		virtual void Render(sf::RenderTarget& target, sf::RenderStates states) = 0;
		virtual void PreRender(EntityManager& manager, UUID id) = 0;
	};

    struct Renderable
    {
        UUID m_ID;
        int m_Layer;
		IRenderable* m_Component;
    };
}
