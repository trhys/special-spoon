#pragma once
#include "Core/Registers/StateRegistry.h"
#include <cstring>

namespace Spoon
{
	namespace DirectionHandler
	{
		inline uint32_t GetIdleStateID(const char* direction)
		{
            if (strcmp(direction, "Up") == 0)
                return BuiltInStates::IdleUp;
            else if (strcmp(direction, "Down") == 0)
                return BuiltInStates::IdleDown;
            else if (strcmp(direction, "Left") == 0)
                return BuiltInStates::IdleLeft;
            else if (strcmp(direction, "Right") == 0)
                return BuiltInStates::IdleRight;
            else if (strcmp(direction, "UpRight") == 0)
                return BuiltInStates::IdleUR;
            else if (strcmp(direction, "UpLeft") == 0)
                return BuiltInStates::IdleUL;
            else if (strcmp(direction, "DownRight") == 0)
                return BuiltInStates::IdleDR;
            else if (strcmp(direction, "DownLeft") == 0)
                return BuiltInStates::IdleDL;
            else
                return BuiltInStates::Idle;
		}

        inline uint32_t GetMovementStateID(const char* direction)
        {
            if (std::strcmp(direction, "Up") == 0)        return BuiltInStates::MoveUp;
            if (std::strcmp(direction, "Down") == 0)      return BuiltInStates::MoveDown;
            if (std::strcmp(direction, "Left") == 0)      return BuiltInStates::MoveLeft;
            if (std::strcmp(direction, "Right") == 0)     return BuiltInStates::MoveRight;
            if (std::strcmp(direction, "UpRight") == 0)   return BuiltInStates::MoveUR;
            if (std::strcmp(direction, "UpLeft") == 0)    return BuiltInStates::MoveUL;
            if (std::strcmp(direction, "DownRight") == 0) return BuiltInStates::MoveDR;
            if (std::strcmp(direction, "DownLeft") == 0)  return BuiltInStates::MoveDL;

            return BuiltInStates::Idle;
        }

	}
}