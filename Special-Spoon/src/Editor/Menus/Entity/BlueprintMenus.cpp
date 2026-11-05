#include "Editor/Editor.h"
#include "Editor/Blueprints/Blueprint.h"

namespace Spoon
{
    void Editor::DrawBlueprintsMenu()
    {
        if (!m_ShowBlueprintsMenu)
            return;

        static bool error = false;
        const char* errorMsg = "An error occurred while accessing blueprints. Check logs";
        if (ImGui::Begin("Blueprints", &m_ShowBlueprintsMenu))
        {
            if (!m_CurrentProject)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "No project loaded");
                ImGui::End();
                return;
            }
            
            if (ImGui::IsWindowAppearing())
                error = false;
            if (error)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorMsg);
            }
            static std::string selectedBlueprint;
            // draw a table with the first column as available blueprints
            //  and the second column as their components
            if (ImGui::BeginTable("BlueprintsTable", 2))
            {
                ImGui::TableSetupColumn("Blueprint");
                ImGui::TableSetupColumn("Components");
                ImGui::TableHeadersRow();

                // iterate blueprints directory for blueprint files
                const auto blueprintsDir = m_Services.projectManager->GetCurrentProjectPaths().Blueprints();

                // validate dir and iterate
                try 
                {
                    if (std::filesystem::create_directory(blueprintsDir))
                    {
                        SS_DEBUG_LOG("[BLUEPRINTS] Created blueprints directory")
                    }
                    
                    for (const auto& blueprint : std::filesystem::directory_iterator(blueprintsDir))
                    {
                        if (!blueprint.is_regular_file() || blueprint.path().extension() != ".json")
                            continue;

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        if (ImGui::Selectable(blueprint.path().filename().string().c_str()))
                        {
                            selectedBlueprint = blueprint.path().filename().string();
                        }
                    }
                    if (!selectedBlueprint.empty()) // display components of the selected blueprint
                    {
                        ImGui::TableSetColumnIndex(1);
                        const auto blueprintPath = blueprintsDir / selectedBlueprint;
                        std::ifstream blueprintFile(blueprintPath);
                        if (!blueprintFile.is_open())
                            throw std::filesystem::filesystem_error("Failed to open blueprint file", std::error_code());

                        // read json
                        try {
                            json blueprintJson;
                            blueprintFile >> blueprintJson;
                            EntityBlueprint entityBlueprint = blueprintJson.get<EntityBlueprint>();
                            
                            // read the components of the entity blueprint
                            // and view their defaults
                            for (const auto& comp : entityBlueprint.m_Components)
                            {
                                ImGui::Text("%s", comp.m_Name.c_str());
                                for (const auto& [key, value] : comp.m_Data.items())
                                {
                                    ImGui::Text("  %s: %s", key.c_str(), value.dump().c_str());
                                }
                            }
                        }
                        catch (const json::exception& e)
                        {
                            SS_DEBUG_LOG("[BLUEPRINTS] Failed to parse blueprint JSON: " + std::string(e.what()))
                            error = true;
                        }
                    }
                }
                catch (const std::filesystem::filesystem_error& e)
                {
                    SS_DEBUG_LOG("[BLUEPRINTS] Failed to access blueprint file: " + std::string(e.what()))
                    error = true;
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}
