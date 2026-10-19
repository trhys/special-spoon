#include "Editor/Editor.h"

namespace Spoon
{
    void Editor::DrawBlueprintsMenu()
    {
        if (!m_ShowBlueprintsMenu)
            return;

        static std::string selectedBlueprintID;
        static char blueprintName[64] = "";
        static std::unordered_map<std::string, bool> componentSelections;
        static ImGuiTextFilter componentFilter;
        auto& blueprints = m_Services.projectManager->GetBlueprints();

        ImGui::SetNextWindowSize(ImVec2(760, 520), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Blueprint Creator"))
        {
            ImGui::End();
            return;
        }

        if (ImGui::BeginTable(
            "##BlueprintLayout",
            2,
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("Blueprints", ImGuiTableColumnFlags_WidthFixed, 220.0f);
            ImGui::TableSetupColumn("Editor", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::SeparatorText("Existing Blueprints");
            if (ImGui::Button("New Blueprint", ImVec2(-FLT_MIN, 0)))
            {
                selectedBlueprintID.clear();
                blueprintName[0] = '\0';
                componentSelections.clear();
                componentFilter.Clear();
            }

            if (ImGui::BeginChild("##BlueprintList", ImVec2(0, 0), ImGuiChildFlags_Borders))
            {
                for (std::size_t index = 0; index < blueprints.size(); ++index)
                {
                    const Blueprint& blueprint = blueprints[index];
                    const std::string& name = blueprint.GetDisplayName();
                    const bool selected = selectedBlueprintID == name;
                    ImGui::PushID(static_cast<int>(index));
                    if (ImGui::Selectable(name.empty() ? "<Unnamed Blueprint>" : name.c_str(), selected))
                    {
                        selectedBlueprintID = name;
                        std::snprintf(blueprintName, IM_ARRAYSIZE(blueprintName), "%s", name.c_str());
                        componentSelections.clear();
                        for (const std::string& component : blueprint.GetComps())
                        {
                            componentSelections[component] = true;
                        }
                        componentFilter.Clear();
                    }
                    if (selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();

            ImGui::TableSetColumnIndex(1);
            const auto selectedBlueprint = std::find_if(
                blueprints.begin(),
                blueprints.end(),
                [&](const Blueprint& blueprint)
                {
                    return blueprint.GetDisplayName() == selectedBlueprintID;
                }
            );
            const std::size_t selectedBlueprintIndex =
                selectedBlueprint == blueprints.end()
                    ? blueprints.size()
                    : static_cast<std::size_t>(std::distance(blueprints.begin(), selectedBlueprint));
            const bool editing = selectedBlueprintIndex < blueprints.size();
            ImGui::SeparatorText(editing ? "Edit Blueprint" : "Create Blueprint");
            ImGui::InputText("Name", blueprintName, IM_ARRAYSIZE(blueprintName));

            ImGui::Spacing();
            ImGui::TextUnformatted("Components");
            componentFilter.Draw("Search##BlueprintComponents", -FLT_MIN);

            if (ImGui::BeginChild("##BlueprintComponents", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2.0f), ImGuiChildFlags_Borders))
            {
                for (const auto& [type, array] : m_Services.entityManager->GetAllArrays())
                {
                    if (!componentFilter.PassFilter(type.c_str()))
                        continue;

                    ImGui::PushID(type.c_str());
                    ImGui::Checkbox(type.c_str(), &componentSelections[type]);
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();

            const std::string enteredName(blueprintName);
            const bool duplicateName = std::any_of(
                blueprints.begin(),
                blueprints.end(),
                [&](const Blueprint& blueprint)
                {
                    return blueprint.GetDisplayName() == enteredName &&
                        (!editing || &blueprint != &blueprints[selectedBlueprintIndex]);
                }
            );
            const bool canSave = !enteredName.empty() && !duplicateName;

            ImGui::BeginDisabled(!canSave);
            const bool saveBlueprint =
                ImGui::Button(editing ? "Save Changes" : "Create Blueprint", ImVec2(140, 0));
            ImGui::EndDisabled();
            if (saveBlueprint)
            {
                std::vector<std::string> compsToAdd;
                for (const auto& [comp, selected] : componentSelections)
                {
                    if (selected)
                    {
                        compsToAdd.push_back(comp);
                    }
                }

                if (editing)
                {
                    auto& blueprint = blueprints[selectedBlueprintIndex];
                    blueprint.SetName(enteredName);
                    blueprint.SetComponents(compsToAdd);
                }
                else
                {
                    blueprints.emplace_back(enteredName, compsToAdd);
                }
                selectedBlueprintID = enteredName;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(100, 0)))
            {
                selectedBlueprintID.clear();
                blueprintName[0] = '\0';
                componentSelections.clear();
                componentFilter.Clear();
            }

            if (editing)
            {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                if (ImGui::Button("Delete", ImVec2(100, 0)))
                {
                    blueprints.erase(selectedBlueprint);
                    selectedBlueprintID.clear();
                    blueprintName[0] = '\0';
                    componentSelections.clear();
                    componentFilter.Clear();
                }
                ImGui::PopStyleColor();
            }
            if (enteredName.empty())
                ImGui::TextDisabled("Blueprint name cannot be empty.");
            else if (duplicateName)
                ImGui::TextDisabled("Blueprint names must be unique.");
            ImGui::EndTable();
        }
        ImGui::End();
    }
}