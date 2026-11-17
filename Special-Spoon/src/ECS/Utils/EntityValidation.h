#pragma once

#include "Core/Core.h"
#include "ECS/UUID.h"

namespace Spoon
{
	struct StatusComp;
	struct TransformComp;
	template<typename T>
	struct ComponentArray;

	// validate that an entity has both a StatusComp and TransformComp, and return them if so
	bool SPOON_API HasBase(
		StatusComp*& statusComp,
		TransformComp*& transform,
		ComponentArray<StatusComp>& statusArray,
		ComponentArray<TransformComp>& transformArray,
		UUID id
	);
}