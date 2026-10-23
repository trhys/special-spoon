#include "AttackSerialization.h"
#include "AttackRegistry.h"
#include "ECS/Component/Combat/AttackComp.h"

namespace Attack {
    // project helpers for the Attack Editor
    std::filesystem::path GetAttacksDirectory()
    {
        auto& projectManager = Spoon::Application::Get().GetProjectManager();
        const auto* project = projectManager.GetCurrentProject();
        if (!project)
            return {};
        std::filesystem::create_directories(project->presetsPath / "attacks");
        return project->presetsPath / "attacks";
    }

    // get list of attacks in the attacks directory
    std::vector<std::string> GetAttackIds()
    {
        std::vector<std::string> attackIds;
        auto attacksDir = GetAttacksDirectory();
        if (attacksDir.empty())
            return attackIds;
        for (const auto& entry : std::filesystem::directory_iterator(attacksDir))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                attackIds.push_back(entry.path().stem().string());
            }
        }
        return attackIds;
    }

    // validate current attack fields before serializing 
    bool ValidateAttack(const AttackDef& attack, AttackComp& comp)
    {
        if (attack.id.empty())
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Attack ID cannot be empty.";
            return false;
        }
        if (attack.name.empty())
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Attack name cannot be empty.";
            return false;
        }
        if (attack.halfExtents.x < 0.0f || attack.halfExtents.y < 0.0f)
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Hitbox half-extents cannot be negative.";
            return false;
        }
        if (attack.damage < 0.0f)
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Damage cannot be negative.";
            return false;
        }
        if (attack.lifetimeSec < 0.0f)
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Attack lifetime cannot be negative.";
            return false;
        }
        if (attack.cooldownSec < 0.0f)
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Attack cooldown cannot be negative.";
            return false;
        }
        if (attack.windupSec < 0.0f)
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Attack windup cannot be negative.";
            return false;
        }
        if (attack.activeSec < 0.0f)
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Attack active duration cannot be negative.";
            return false;
        }
        if (attack.recoverySec < 0.0f)
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Attack recovery cannot be negative.";
            return false;
        }

        comp.showErrorMessage = false;
        comp.editorErrorMessage.clear();
        return true;
    }

    // serialize current attack from editor
    void SaveAttack(const AttackDef& attack, AttackComp& comp)
    {
        if (!ValidateAttack(attack, comp))
            return;

        auto attacksDir = GetAttacksDirectory();
        if (attacksDir.empty())
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Failed to get attacks directory.";
            return;
        }
        std::ofstream file(attacksDir / (attack.id + ".json"), std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Failed to open attack file for writing.";
            return;
        }
        nlohmann::json j = attack;
        file << j.dump(4);
        comp.showErrorMessage = false;
        comp.editorErrorMessage.clear();
    }

    void LoadAttack(const std::string& attackId, AttackComp& comp, AttackDef& attack)
    {
        auto attacksDir = GetAttacksDirectory();
        if (attacksDir.empty())
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Failed to get attacks directory.";
            return;
        }
        std::ifstream file(attacksDir / (attackId + ".json"), std::ios::in);
        if (!file.is_open())
        {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Failed to open attack file for reading.";
            return;
        }
        nlohmann::json j;
        try {
            file >> j;
            attack = j.get<AttackDef>();
        } catch (const std::exception& e) {
            comp.showErrorMessage = true;
            comp.editorErrorMessage = "Failed to parse attack JSON.";
            return;
        }
        comp.showErrorMessage = false;
        comp.editorErrorMessage.clear();
    }
}

namespace init {
    void InitializeAttackRegistry()
    {
        SS_DEBUG_LOG("[INIT] Initializing Attack Registry");
        // register each attack from the project->presets/attacks
        // directory into the static attack registry
        auto attacksDir = Attack::GetAttacksDirectory();
        if (!attacksDir.empty())
        {
            for (const auto& entry : std::filesystem::directory_iterator(attacksDir))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".json")
                {
                    std::ifstream file(entry.path(), std::ios::in);
                    if (file.is_open())
                    {
                        nlohmann::json j;
                        try {
                            file >> j;
                            AttackDef attack = j.get<AttackDef>();
                            AttackRegistry::Get().RegisterAttack(attack);
                        } catch (const std::exception& e) {
                            SS_DEBUG_LOG(std::string("[INIT] Failed to parse attack JSON: ") + e.what());
                        }
                    }
                }
            }
        } else {
            SS_DEBUG_LOG("[INIT] Attacks directory is empty or could not be found.");
        }
    }
}