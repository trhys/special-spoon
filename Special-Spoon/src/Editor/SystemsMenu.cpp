#include "SystemsMenu.h"
#include "System/SystemManager.h"

namespace Spoon
{
    void SystemsMenu::Open()
    {
        isOpen = true;
    }

    bool SystemsMenu::IsOpen()
    {
        return isOpen;
    }

    void SystemsMenu::ReloadSystems()
    {
        init = true;
    }

    void SystemsMenu::Update(SystemManager& manager)
    {
        static std::unordered_map<std::string, bool> addedSystems;
        static bool editedSystems = false;
        static std::vector<std::string> existing;

        if(init) // Inform the editor of systems that may get loaded by the scene manager elsewhere
        {
            for (auto& system : manager.GetSystems())
            {
                std::string id = system->GetDisplayName();
                addedSystems[id] = true;
                existing.push_back(id);
            }
            init = false;
        }

        ImGui::Begin("System Manager", &isOpen, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        if (ImGui::BeginChild("System Selector", ImVec2(0, 200)))
        {
            for (const auto& [id, loader] : SystemLoaders::GetSysLoaders())
            {
                if (ImGui::Checkbox(id.c_str(), &addedSystems[id]))
                {
                    if(addedSystems[id])
                        existing.push_back(id);
                    else
                        existing.erase(std::remove(existing.begin(), existing.end(), id), existing.end());
                    editedSystems = true;
                }
            }
            ImGui::EndChild();
        }
        if (editedSystems)
        {
            // Remove systems that are unchecked
            auto& systems = manager.GetSystems();
            systems.erase(std::remove_if(systems.begin(), systems.end(),
                [&](const std::unique_ptr<ISystem>& sys)
                {
                    return !addedSystems[sys->GetDisplayName()];
                }),
                systems.end());

            // Add checked systems that aren't aleady here
            auto& loaders = SystemLoaders::GetSysLoaders();
            for (auto& id : existing)
            {
                bool exists = std::any_of(systems.begin(), systems.end(),
                    [&](auto& sys){ return sys->GetDisplayName() == id; });
                if (!exists)
                {
                    manager.AddSystem(id);
                }
            }

            // Sort by index in existing - this gets modified in the drag and drop below
            std::sort(systems.begin(), systems.end(), [&](const auto& a, const auto& b) {
                auto itA = std::find(existing.begin(), existing.end(), a->GetDisplayName());
                auto itB = std::find(existing.begin(), existing.end(), b->GetDisplayName());
                return itA < itB;
            });

            editedSystems = false;
        }
        if (ImGui::BeginChild("Active Systems", ImVec2(0, 200)))
        {
            if (ImGui::BeginListBox("##Systems"))
            {
                for (int index = 0; index < existing.size(); index++)
                {
                    const std::string& id = existing[index];
                    ImGui::PushID(id.c_str());
                    if (ImGui::Selectable(id.c_str()))
                    {
                        // Placeholder - may put something here later
                    }

                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                    {
                        ImGui::SetDragDropPayload("Reorder systems", &index, sizeof(int));
                        ImGui::Text("Moving: %s", id.c_str());
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Reorder systems"))
                        {
                            int movefrom = *(const int*)payload->Data;
                            int moveto = index;

                            std::string item = existing[movefrom];
                            existing.erase(existing.begin() + movefrom);
                            existing.insert(existing.begin() + moveto, item);

                            editedSystems = true;
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}