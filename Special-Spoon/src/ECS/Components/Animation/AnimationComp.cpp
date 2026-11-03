#include "AnimationComp.h"
#include "Core/Application.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "Editor/Utils/Windows.h"

namespace Spoon
{
    template<std::size_t N>
    void DrawStateSelector(char (&buffer)[N])
    {
        if (ImGui::BeginCombo("State ID", buffer[0] ? buffer : "Select State"))
        {
            for (const auto& [name, id] : StateRegistry::Get().m_NameToID)
            {
                if (ImGui::Selectable(name.c_str()))
                {
                    strncpy(buffer, name.c_str(), N - 1);
                    buffer[N - 1] = '\0';
                }
            }
            ImGui::EndCombo();
        }
    }

    template<std::size_t N>
    void DrawAnimationSelector(char (&buffer)[N], std::filesystem::path& path)
    {
        if (ImGui::BeginCombo("Animation ID", buffer[0] ? buffer : "Select Animation"))
        {
            // here we'll iterate the project's assets/animations directory
            // for available animation data, and lazy load it once confirmed
            auto animationsPath = Application::Get().GetProjectManager().GetAnimationsPath();
            if (std::filesystem::exists(animationsPath))
            {
                for (const auto& file : std::filesystem::directory_iterator(animationsPath))
                {
                    if (!file.is_regular_file() || file.path().extension() != ".json")
                        continue;

                    const auto& name = file.path().stem().string();
                    if (ImGui::Selectable(name.c_str()))
                    {
                        path = file.path();
                        strncpy(buffer, name.c_str(), N - 1);
                        buffer[N - 1] = '\0';
                    }
                }
            }
            ImGui::EndCombo();
        }
    }

    void AnimationComp::SetAnimationData(const std::string animationID)
    {
        m_AnimationData = ResourceManager::Get().GetAnimationData(animationID);
    }

    void AnimationComp::OnReflect()
    {
        static std::string deletedState = "";
        static std::string editingState = "";
        static char editStateIDbuf[64];
        static char editAnimationIDbuf[64];
        static std::filesystem::path editSelectedAnimationPath;

        bool openEditPopup = false;
        bool openDeletePopup = false;

        if (ImGui::BeginChild("Animation Map", ImVec2(0, 250), ImGuiChildFlags_Borders))
        {
            ImGui::SeparatorText("Animation Map");
            for (auto& [state, animation] : m_AnimationMap)
            {
                ImGui::PushID(state.c_str());
                ImGui::Text("State ID: %s", state.c_str());
                ImGui::Text("Animation ID: %s", animation.c_str());
                if (ImGui::Button("Edit"))
                {
                    editingState = state;
                    strncpy(editStateIDbuf, state.c_str(), sizeof(editStateIDbuf));
                    strncpy(editAnimationIDbuf, animation.c_str(), sizeof(editAnimationIDbuf));
                    editSelectedAnimationPath.clear();
                    openEditPopup = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete"))
                {
                    deletedState = state;
                    openDeletePopup = true;
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        if (openEditPopup)
            ImGui::OpenPopup("Edit Animation Mapping");
        if (openDeletePopup)
            ImGui::OpenPopup("Delete Animation Mapping");

        CenterNext();
        if (ImGui::BeginPopupModal("Edit Animation Mapping"))
        {
            DrawStateSelector<64>(editStateIDbuf);
            ImGui::SameLine();
            DrawAnimationSelector<64>(editAnimationIDbuf, editSelectedAnimationPath);

            if (ImGui::Button("Submit"))
            {
                if (editStateIDbuf[0] != '\0' && editAnimationIDbuf[0] != '\0')
                {
                    bool loaded = true;
                    if (!editSelectedAnimationPath.empty())
                        loaded = ResourceManager::Get().LoadAnimationFromPath(editSelectedAnimationPath);

                    if (loaded)
                    {
                        m_AnimationMap.erase(editingState);
                        m_AnimationMap[editStateIDbuf] = editAnimationIDbuf;
                        editStateIDbuf[0] = '\0';
                        editAnimationIDbuf[0] = '\0';
                        editSelectedAnimationPath.clear();
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                editStateIDbuf[0] = '\0';
                editAnimationIDbuf[0] = '\0';
                editSelectedAnimationPath.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        CenterNext();
        if (ImGui::BeginPopupModal("Delete Animation Mapping"))
        {
            ImGui::Text("Are you sure you want to\ndelete this mapping? This cannot be undone!");
            if (ImGui::Button("Delete")) 
            { 
                m_AnimationMap.erase(deletedState);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine(); if(ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (ImGui::Button("Add New"))
        {
            ImGui::OpenPopup("Add New Animation Mapping");
        }

        CenterNext();
        if (ImGui::BeginPopupModal("Add New Animation Mapping"))
        {
            static char newStateIDbuf[64];
            static char newAnimationIDbuf[64];
            static std::filesystem::path selectedAnimationPath;

            DrawStateSelector<64>(newStateIDbuf);
            ImGui::SameLine();
            DrawAnimationSelector<64>(newAnimationIDbuf, selectedAnimationPath);

            if (ImGui::Button("Submit"))
            {
                if (newStateIDbuf[0] != '\0' && newAnimationIDbuf[0] != '\0' && !selectedAnimationPath.empty())
                {
                    // load the animation data from the selected path
                    bool loaded = ResourceManager::Get().LoadAnimationFromPath(selectedAnimationPath);
                    if (loaded)
                    {
                        m_AnimationMap[newStateIDbuf] = newAnimationIDbuf;
                        newStateIDbuf[0] = '\0';
                        newAnimationIDbuf[0] = '\0';
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                newStateIDbuf[0] = '\0';
                newAnimationIDbuf[0] = '\0';
                selectedAnimationPath.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}