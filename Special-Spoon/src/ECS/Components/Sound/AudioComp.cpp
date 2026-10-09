#include "AudioComp.h"
#include "Core/ResourceManager/ResourceManager.h"

namespace Spoon
{
    void AudioComp::PlaySound() {
        if (sound.has_value())
            sound->play();
    }

    void AudioComp::StopSound() {
        if (sound.has_value())
            sound->stop();
    }

    void AudioComp::LoadSound(const std::string& fileId) {
        sound = sf::Sound(ResourceManager::Get().GetResource<sf::SoundBuffer>(fileId));
    }

    bool AudioComp::IsActionMapped(uint32_t id)
    {
      auto found = actionBuffers.find(id);
      if (found == actionBuffers.end())
      {
        return false;
      }
      return true;
    }

    void AudioComp::OnReflect() {
        // listbox of available sound buffers in the resource manager
        ImGui::TextDisabled("Select the sound buffer to use for this audio component");
        if (ImGui::BeginCombo("Sound Buffer", bufferId.c_str()))
        {
            for (const auto& [id, resource] : ResourceManager::Get().GetSounds())
            {
                bool isSelected = (bufferId == id);
                if (ImGui::Selectable(id.c_str(), isSelected))
                    bufferId = id;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Play Sound"))
        {
          if (bufferId != "")
            this->LoadSound(bufferId);
          this->PlaySound();
        }

        ImGui::TextDisabled("Sound Settings");
        ImGui::SliderFloat("Volume", &volume, 0.0f, 100.0f); ImGui::SameLine();
        HelpMarker("Adjust the volume of the sound (0 = silent, 100 = full volume)");

        ImGui::Separator();
        ImGui::TextDisabled("Action -> Buffer Mappings");

        // local editor state
        static uint32_t currentAction = 0;
        static uint32_t editAction = 0;
        static std::string editBuffer;
        static std::string newActionName;

        // list current mappings
        if (!actionBuffers.empty())
        {
            for (auto it = actionBuffers.begin(); it != actionBuffers.end(); ++it)
            {
                const uint32_t actionId = it->first;
                const std::string& mappedBuffer = it->second;

                std::string actionName = "<Unknown>";
                try
                {
                    actionName = ActionRegistry::Get().GetName(ActionType{ actionId });
                }
                catch (...) {}

                ImGui::PushID((int)actionId);
                ImGui::Text("%s -> %s", actionName.c_str(), mappedBuffer.c_str());
                ImGui::SameLine();

                if (ImGui::Button("Edit"))
                {
                    currentAction = actionId;
                    editAction = actionId;
                    editBuffer = mappedBuffer;
                    ImGui::OpenPopup("Edit Audio Mapping");
                }
                ImGui::SameLine();

                if (ImGui::Button("Delete"))
                {
                    it = actionBuffers.erase(it);
                    ImGui::PopID();
                    if (it == actionBuffers.end())
                        break;
                    continue;
                }
                ImGui::PopID();
            }

            if (ImGui::BeginPopup("Edit Audio Mapping"))
            {
                ImGui::BeginChild("Available Actions##edit", ImVec2(220, 260), true);
                if (ImGui::BeginListBox("##audio_action_list_edit", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for (const auto& [name, id] : ActionRegistry::Get().m_NameToID)
                    {
                        const bool is_selected = (editAction == id);
                        ImGui::PushID((int)id);
                        if (ImGui::Selectable(name.c_str(), is_selected))
                            editAction = id;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();

                ImGui::SameLine();

                ImGui::BeginChild("Available Buffers##edit", ImVec2(260, 260), true);
                if (ImGui::BeginListBox("##audio_buffer_list_edit", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for (const auto& [id, resource] : ResourceManager::Get().GetSounds())
                    {
                        const bool is_selected = (editBuffer == id);
                        ImGui::PushID(id.c_str());
                        if (ImGui::Selectable(id.c_str(), is_selected))
                            editBuffer = id;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();

                if (ImGui::Button("Submit"))
                {
                    if (currentAction != 0)
                        actionBuffers.erase(currentAction);

                    if (editAction != 0 && !editBuffer.empty())
                        actionBuffers[editAction] = editBuffer;

                    currentAction = 0;
                    editAction = 0;
                    editBuffer.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    currentAction = 0;
                    editAction = 0;
                    editBuffer.clear();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }
        else
        {
            ImGui::TextDisabled("No action mappings yet");
        }

        // new mapping popup
        if (ImGui::Button("Add New Mapping"))
        {
            editAction = 0;
            editBuffer.clear();
            ImGui::OpenPopup("New Audio Mapping");
        }

        if (ImGui::BeginPopup("New Audio Mapping"))
        {
            ImGui::BeginChild("Available Actions##new", ImVec2(220, 260), true);
            if (ImGui::BeginListBox("##audio_action_list_new", ImVec2(-FLT_MIN, -FLT_MIN)))
            {
                for (const auto& [name, id] : ActionRegistry::Get().m_NameToID)
                {
                    const bool is_selected = (editAction == id);
                    ImGui::PushID((int)id);
                    if (ImGui::Selectable(name.c_str(), is_selected))
                        editAction = id;
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("Available Buffers##new", ImVec2(260, 260), true);
            if (ImGui::BeginListBox("##audio_buffer_list_new", ImVec2(-FLT_MIN, -FLT_MIN)))
            {
                for (const auto& [id, resource] : ResourceManager::Get().GetSounds())
                {
                    const bool is_selected = (editBuffer == id);
                    ImGui::PushID(id.c_str());
                    if (ImGui::Selectable(id.c_str(), is_selected))
                        editBuffer = id;
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
            ImGui::EndChild();

            if (ImGui::Button("Submit"))
            {
                if (editAction != 0 && !editBuffer.empty())
                    actionBuffers[editAction] = editBuffer;

                editAction = 0;
                editBuffer.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                editAction = 0;
                editBuffer.clear();
                ImGui::CloseCurrentPopup();
            }

            ImGui::Separator();
            ImGui::TextDisabled("Create custom action");
            static char actionNameBuf[128] = "";
            ImGui::InputText("Action Name", actionNameBuf, IM_ARRAYSIZE(actionNameBuf));
            if (ImGui::Button("Create Action"))
            {
                std::string name = actionNameBuf;
                if (!name.empty() && ActionRegistry::Get().m_NameToID.find(name) == ActionRegistry::Get().m_NameToID.end())
                {
                    ActionRegistry::Get().RegisterCustom(name);
                    memset(actionNameBuf, 0, sizeof(actionNameBuf));
                }
            }

            ImGui::EndPopup();
        }
    }
}
