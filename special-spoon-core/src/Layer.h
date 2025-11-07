#pragma once

#include "Core.h"
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

		std::vector<Entity*>& GetEntities() { return m_Entities; }
		void AddEntity(Entity* entity);
		void KillEntity(Entity* entity);

	private:
		std::vector<Entity*> m_Entities;

	};
}