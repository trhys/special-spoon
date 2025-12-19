#pragma once

#include "Spoon.h"

struct PlayerComp : public Spoon::ComponentBase<PlayerComp>
{
    PlayerComp() : ComponentBase::ComponentBase("PlayerComp") {}
};