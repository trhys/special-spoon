#include "EntityValidation.h"
#include "Core/ComponentArray.h"
#include "ECS/Components/Core/TransformComp.h"
#include "ECS/Components/Core/StatusComp.h"

namespace Spoon
{
	bool HasBase(
		StatusComp*& statusComp,
		TransformComp*& transform,
		ComponentArray<StatusComp>& statusArray,
		ComponentArray<TransformComp>& transformArray,
		UUID id
	) {
		if (statusArray.m_IdToIndex.count(id))
		{
			statusComp = &statusArray.m_Components[statusArray.m_IdToIndex[id]];
		}
		else // missing status comp
			return false;

		if (transformArray.m_IdToIndex.count(id))
		{
			transform = &transformArray.m_Components[transformArray.m_IdToIndex[id]];
		}
		else // missing transform comp
			return false;

		return true;
	}
}