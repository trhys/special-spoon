#pragma once

#include "Core.h"
#include "Scene.h"
#include "Entity.h"

namespace Spoon { 

	class SPOON_API Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual bool OnEvent(const sf::Event& e) { return false; }

		void DrawScene(sf::RenderTarget& target, sf::Transform& scene_transform) { m_Scene.draw(target, scene_transform) }

	private:
		Scene* m_Scene;

	};
}