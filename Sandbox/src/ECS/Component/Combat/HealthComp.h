#pragma once

#include "Spoon.h"

// helper struct for queueing damage events
struct DamageRequest {
    float damageAmount = 0.0f;   // amount of damage to apply
};

struct HealthComp : public Spoon::ComponentBase<HealthComp> {
    HealthComp() : Spoon::ComponentBase<HealthComp>(Name) {}

    static constexpr const char* Name = "Health";

    std::queue<DamageRequest> damageQueue;
    bool isAlive = true;
    int m_CurrentHealth = 100;
    bool isInvincible = false;
    float invincibilityTimer = 0.0f;

    int m_MaxHealth = 100;
    float iFramesDuration = 0.1f;

    void QueueDamage(float amount)
    {
        damageQueue.push({ amount });
    }
    
    void OnReflect() override
    {
        ImGui::TextDisabled("Current Health: %d", m_CurrentHealth);
        ImGui::TextDisabled("Alive: %s", isAlive ? "Yes" : "No");
        ImGui::TextDisabled("Invincible: %s", isInvincible ? "Yes" : "No");
        ImGui::TextDisabled("Invincibility Timer: %.2f", invincibilityTimer);

        ImGui::Separator();
        ImGui::InputInt("Max Health", &m_MaxHealth);
        ImGui::InputFloat("iFrames Duration", &iFramesDuration); ImGui::SameLine();
        Spoon::HelpMarker("Duration of invincibility frames after taking damage.");
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HealthComp, m_MaxHealth, iFramesDuration, isInvincible)

