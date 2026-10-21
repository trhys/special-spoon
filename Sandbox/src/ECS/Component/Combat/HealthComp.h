#pragma once

#include "Spoon.h"

struct HealthComp : public Spoon::ComponentBase<HealthComp> {
    HealthComp() : Spoon::ComponentBase<HealthComp>(Name) {}

    static constexpr const char* Name = "Health";

    // runtime
    bool isAlive = true;
    int m_CurrentHealth = 100;

    // config
    int m_MaxHealth = 100;


    void OnReflect() override
    {
        ImGui::TextDisabled("Current Health: %d", m_CurrentHealth);
        ImGui::TextDisabled("Alive: %s", isAlive ? "Yes" : "No");

        ImGui::Separator();
        ImGui::InputInt("Max Health", &m_MaxHealth);
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HealthComp, m_MaxHealth)

