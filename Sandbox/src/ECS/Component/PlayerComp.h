#pragma once

#include "Spoon.h"

struct PlayerComp : public Spoon::ComponentBase<PlayerComp>
{
    PlayerComp() : ComponentBase::ComponentBase("PlayerComp") {}

    bool empty;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerComp, empty)