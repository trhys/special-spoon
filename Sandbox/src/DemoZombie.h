#pragma once

#include "Spoon.h"

class DemoZombie : public Spoon::Entity
{
public:
    DemoZombie();
    ~DemoZombie() {}

    void OnAdd() override;
    void OnKill() override;

};