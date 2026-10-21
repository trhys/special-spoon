#pragma once

#include "Spoon.h"

struct RespawnComp : public Spoon::ComponentBase<RespawnComp>
{
    RespawnComp() : Spoon::ComponentBase<RespawnComp>(Name) {}
  
    static constexpr const char* Name = "Respawn";

    sf::Vector2f respawnPosition = {0.0f, 0.0f}; // Position where the entity will respawn
    float respawnTime = 5.0f; // Default respawn time in seconds
    float respawnTimer = 0.0f; // Timer to track respawn countdown
    bool isRespawning = false; // Flag to indicate if the entity is currently respawning

    void OnReflect() override
    {
        ImGui::TextDisabled("Is Respawning: %s", isRespawning ? "Yes" : "No");
        ImGui::TextDisabled("Respawn Timer: %.2f", respawnTimer);
        ImGui::InputFloat("Respawn Time", &respawnTime);
        ImGui::SliderFloat2("Respawn Position", &respawnPosition.x, -1000.0f, 1000.0f);
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RespawnComp, respawnPosition, respawnTime)