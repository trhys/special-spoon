#include "ECS/Component/Combat/AttackComp.h"
#include "ECS/Component/Combat/Attack.h"

namespace {
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

void AttackComp::DrawAttackEditor()
{
    static AttackDef attack{};
    static std::string selectedId;
    static char id[128] = "";
    static char name[128] = "";
    if (ImGui::Begin("Attack Editor", &showAttackEditor, ImGuiWindowFlags_MenuBar))
    {
        // menu bar for file ops
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    attack = AttackDef{};
                    std::memset(id, 0, sizeof(id));
                    std::memset(name, 0, sizeof(name));
                }
                if (ImGui::MenuItem("Load"))
                {
                    ImGui::OpenPopup("Select Attack");
                }
                if (ImGui::MenuItem("Save"))
                {
                    SaveAttack(attack, *this);
                }
                if (ImGui::MenuItem("Delete"))
                {
                    ImGui::OpenPopup("Select Attack");
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // show error if save failed validation
        if (showErrorMessage)
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", editorErrorMessage.c_str());

        if (ImGui::InputText("ID", id, IM_ARRAYSIZE(id)))
            attack.id = id;
        ImGui::SameLine();
        Spoon::HelpMarker("The unique identifier for the attack.");

        if (ImGui::InputText("Title", name, IM_ARRAYSIZE(name)))
            attack.name = name;
        ImGui::SameLine();
        Spoon::HelpMarker("The display name of the attack.");

        ImGui::SeparatorText("Attack Settings");
        ImGui::SliderFloat2("Hitbox Halfsize", &attack.halfExtents.x, 0.0f, 10.0f); ImGui::SameLine();
        Spoon::HelpMarker("The half-size of the attack hitbox.");
        ImGui::SliderFloat2("Hitbox Offset", &attack.offset.x, -10.0f, 10.0f); ImGui::SameLine();
        Spoon::HelpMarker("The offset of the attack hitbox relative to the entity.");

        ImGui::SeparatorText("Damage Settings");
        ImGui::DragFloat("Damage", &attack.damage, 0.1f, 0.0f, 100.0f); ImGui::SameLine();
        Spoon::HelpMarker("The amount of damage this attack deals.");
        ImGui::DragFloat("Knockback", &attack.knockback, 0.1f, 0.0f, 100.0f); ImGui::SameLine();
        Spoon::HelpMarker("The knockback force applied by this attack.");
        ImGui::DragFloat("Attack Lifetime", &attack.lifetimeSec, 0.1f, 0.0f, 100.0f); ImGui::SameLine();
        Spoon::HelpMarker("The duration of the attack in seconds.");

        ImGui::SeparatorText("Attack Cycle");
        ImGui::DragFloat("Attack Cooldown", &attack.cooldownSec, 0.1f, 0.0f, 100.0f); ImGui::SameLine();
        Spoon::HelpMarker("The cooldown duration of the attack in seconds.");
        ImGui::DragFloat("Attack Windup", &attack.windupSec, 0.1f, 0.0f, 100.0f); ImGui::SameLine();
        Spoon::HelpMarker("The windup duration of the attack in seconds.");
        ImGui::DragFloat("Attack Active", &attack.activeSec, 0.1f, 0.0f, 100.0f); ImGui::SameLine();
        Spoon::HelpMarker("The active duration of the attack in seconds.");
        ImGui::DragFloat("Attack Recovery", &attack.recoverySec, 0.1f, 0.0f, 100.0f); ImGui::SameLine();
        Spoon::HelpMarker("The recovery duration of the attack in seconds.");
        
        ImGui::SeparatorText("Other Settings");
        ImGui::Checkbox("Friendly Fire", &attack.friendlyFire); ImGui::SameLine();
        Spoon::HelpMarker("Whether this attack can hit allies.");
        ImGui::Checkbox("Can Hit Self", &attack.canHitSelf); ImGui::SameLine();
        Spoon::HelpMarker("Whether this attack can hit the entity that created it.");
        ImGui::Checkbox("Follows Owner", &attack.followOwner); ImGui::SameLine();
        Spoon::HelpMarker("Whether this attack follows the entity that created it.");
    }

    // popup modal for attack selection
    if (ImGui::BeginPopupModal("Select Attack", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        auto attackIds = GetAttackIds();
        for (const auto& id : attackIds)
        {
            if (ImGui::Selectable(id.c_str()))
                selectedId = id;
        }
        if (ImGui::Button("Load"))
        {
            if (!selectedId.empty())
            {
                LoadAttack(selectedId, *this, attack);
                std::snprintf(id, sizeof(id), "%s", attack.id.c_str());
                std::snprintf(name, sizeof(name), "%s", attack.name.c_str());
                selectedId.clear();
                ImGui::CloseCurrentPopup();
            }
        } ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            if (!selectedId.empty())
            {
                auto attacksDir = GetAttacksDirectory();
                if (!attacksDir.empty())
                {
                    std::filesystem::remove(attacksDir / (selectedId + ".json"));
                    selectedId.clear();
                    ImGui::CloseCurrentPopup();
                }
            }
        } ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            selectedId.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void AttackComp::OnReflect()
{
    ImGui::Text("Attack id: %s", selectedAttack.c_str());
    ImGui::BeginListBox("Selected Attack");
    for (const auto& id : GetAttackIds())
    {
        if (ImGui::Selectable(id.c_str()))
            selectedAttack = id;
    }
    ImGui::EndListBox();

    if (ImGui::Button("Open Editor"))
        showAttackEditor = true;
    if (showAttackEditor)
        DrawAttackEditor();
}