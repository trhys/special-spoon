#include "Entity.h"
#include "Layer.h"

namespace Spoon
{
	void Layer::AddEntity(Entity* entity)
	{
		entity->OnAdd();
		m_Entities.emplace_back(entity);
	}

	void Layer::KillEntity(Entity* entity)
	{
		entity->OnKill();
		m_Entities.erase(std::remove(m_Entities.begin(), m_Entities.end(), entity), m_Entities.end());
		delete entity;
	}
}
