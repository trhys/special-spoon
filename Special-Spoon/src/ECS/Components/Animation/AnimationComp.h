#pragma once

#include "Core/ResourceManager/ResourceManager.h"
#include "ECS/Components/Component.h"
#include "AnimationData.h"

namespace Spoon
{
    struct AnimationComp : public ComponentBase<AnimationComp>
    {
        AnimationComp() : ComponentBase::ComponentBase("Animation") {}
        AnimationComp(std::string animationID, std::unordered_map<std::string, std::string> animationMap = {}) 
            : ComponentBase::ComponentBase("Animation"), iAnimationID(animationID), m_AnimationMap(animationMap)
        {
            m_AnimationData = ResourceManager::Get().GetAnimationData(animationID);
        }

        AnimationData* m_AnimationData = nullptr;
        std::string iAnimationID;
        std::unordered_map<std::string, std::string> m_AnimationMap; // [key, value] = [state, animationID]

        int currentFrame = 0;
        float elapsedTime = 0.0f;
        bool isFinished = false;

        void SetAnimationData(const std::string animationID)
        {
            m_AnimationData = ResourceManager::Get().GetAnimationData(animationID);
        }

        void OnReflect() override
        {
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            static std::string deletedState = "";
            static std::string editingState = "";
            static char editStateIDbuf[64];
            static char editAnimationIDbuf[64];

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
                        ImGui::OpenPopup("Edit Animation Mapping");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Delete"))
                    {
                        deletedState = state;
                        ImGui::OpenPopup("Delete Animation Mapping");
                    }
                    ImGui::PopID();
                }
                ImGui::EndChild();
            }

            // Always center this window when appearing
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Edit Animation Mapping"))
            {
                ImGui::InputText("State ID", editStateIDbuf, IM_ARRAYSIZE(editStateIDbuf));
                ImGui::InputText("Animation ID", editAnimationIDbuf, IM_ARRAYSIZE(editAnimationIDbuf));
                if (ImGui::Button("Submit"))
                {
                    if(editStateIDbuf[0] != '\0' && editAnimationIDbuf[0] != '\0')
                    m_AnimationMap.erase(editingState);
                    m_AnimationMap[editStateIDbuf] = editAnimationIDbuf;
                    editStateIDbuf[0] = '\0';
                    editAnimationIDbuf[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    editStateIDbuf[0] = '\0';
                    editAnimationIDbuf[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            // Always center this window when appearing
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
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
                // todo
            }
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationComp, iAnimationID, m_AnimationMap)
}