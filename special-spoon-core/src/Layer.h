#pragma once

#include "Entity.h"

namespace Spoon { 

	class Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual bool OnEvent(const sf::Event& e) {}

		std::vector<Entity*>& GetEntities() { return m_Entities; }
		void AddEntity(Entity* entity);
		void KillEntity(Entity* entity);

	private:
		std::vector<Entity*> m_Entities;

	};
}