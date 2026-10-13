#include "InputComp.h"
#include "Core/Application.h"
#include "Editor/Utils/Tooltips.h"

#include <optional>

namespace Spoon
{
    namespace InputCompUtil
    {
        // sfml doesn't provide a native mouse button to string mapping, so we have to do it ourselves
        std::string MouseButtonToString(sf::Mouse::Button button)
        {
            switch (button)
            {
            case sf::Mouse::Button::Left: return "LeftMouseButton";
            case sf::Mouse::Button::Right: return "RightMouseButton";
            case sf::Mouse::Button::Middle: return "MiddleMouseButton";
            case sf::Mouse::Button::Extra1: return "X1MouseButton";
            case sf::Mouse::Button::Extra2: return "X2MouseButton";
            }
			return "UnknownMouseButton";
        }

        std::optional<sf::Mouse::Button>
        StringToMouseButton(const std::string& str)
        {
            if (str == "LeftMouseButton") return sf::Mouse::Button::Left;
            if (str == "RightMouseButton") return sf::Mouse::Button::Right;
            if (str == "MiddleMouseButton") return sf::Mouse::Button::Middle;
            if (str == "X1MouseButton") return sf::Mouse::Button::Extra1;
            if (str == "X2MouseButton") return sf::Mouse::Button::Extra2;
			return std::nullopt;
        }
    }

    void InputComp::OnReflect()
    {
        static sf::Keyboard::Key currentKey = sf::Keyboard::Key::Unknown; // Currently selected key
        static sf::Keyboard::Key editKey = sf::Keyboard::Key::Unknown;    // Key to replace binding
        static uint32_t editAction = 0;                                   // Action ID to replace binding
		static bool submitKeyError = false;                                      // Flag to indicate if there was an error during submission

        ImGui::SeparatorText("Current Key Bindings");
        if(ImGui::BeginListBox("##Key Bindings"))
        {
            for(auto& [key, action] : m_KeyBindings)
            {   
                std::string keyName = sf::Keyboard::getDescription(static_cast<sf::Keyboard::Scancode>(key)).toAnsiString();
                ImGui::PushID(keyName.c_str());
                const bool is_selected = (currentKey == key);
                if(ImGui::Selectable(keyName.c_str(), is_selected))
                {
                    currentKey = key;
                }
                if(is_selected)
                    ImGui::SetItemDefaultFocus();
                ImGui::PopID();
            }
            ImGui::EndListBox();
        }
        ImGui::SameLine(); HelpMarker("Map a key to a registered event. The event is pushed to the action queue to be consumed by some system.");
        
        ImGui::SeparatorText("Action String Mapping");
        if(currentKey != sf::Keyboard::Key::Unknown && m_KeyBindings.count(currentKey))
        {
            const char* editWindow = "Edit Binding";

            std::string keyName = sf::Keyboard::getDescription(static_cast<sf::Keyboard::Scancode>(currentKey)).toAnsiString();
            ImGui::Text("Key: %s", keyName.c_str());
            ImGui::Text("Action: %s", ActionRegistry::Get().GetName(m_KeyBindings[currentKey]).c_str());
            
            if (ImGui::Button("Edit Key Binding"))
            {
                editKey = currentKey;
                editAction = m_KeyBindings[currentKey].m_ID;
                ImGui::OpenPopup(editWindow);
			} ImGui::SameLine();
            if (ImGui::Button("Remove Key Binding"))
            {
                m_KeyBindings.erase(currentKey);
                currentKey = sf::Keyboard::Key::Unknown;
			}

            if(ImGui::BeginPopup(editWindow))
            {   
                ImGui::BeginChild("Available Keys", ImVec2(200, 300), true);
                if (ImGui::BeginListBox("##keylist", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for (int k = static_cast<int>(sf::Keyboard::Key::A); k <= static_cast<int>(sf::Keyboard::KeyCount); k++)
                    {
                        sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(k);
                        if (m_KeyBindings.find(key) != m_KeyBindings.end() && key != editKey)
                            continue;

                        std::string keyName = sf::Keyboard::getDescription(static_cast<sf::Keyboard::Scancode>(key)).toAnsiString();
                        ImGui::PushID(keyName.c_str());
                        const bool is_selected = (editKey == key);
                        if (ImGui::Selectable(keyName.c_str(), is_selected))
                        {
                            editKey = key;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();
                
                ImGui::SameLine();
                
				// display available actions
				AvailableActionsList(editAction, "##actionlist");

                if (submitKeyError)
                {
                    ErrorMessage("Please select a key and an action before submitting.");
				}
                
                if(ImGui::Button("Submit"))
                {
                    if (editKey == sf::Keyboard::Key::Unknown || editAction == 0)
                    {
                        submitKeyError = true;
                    }
                    else 
                    {
                        submitKeyError = false;
                        m_KeyBindings.erase(currentKey);
                        m_KeyBindings[editKey] = ActionType{ editAction };
                        currentKey = sf::Keyboard::Key::Unknown;
                        editAction = 0;
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::SameLine();
                if(ImGui::Button("Cancel"))
                {
                    editAction = 0;
                    editKey = sf::Keyboard::Key::Unknown;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        else
        {
            ImGui::TextDisabled("No mapping available");
        }

        const char* newBindWindow = "New Binding";
        if (ImGui::Button("Add New"))
            ImGui::OpenPopup(newBindWindow);
        if (ImGui::BeginPopup(newBindWindow))
        {
            ImGui::BeginChild("Available Keys", ImVec2(200, 300), true);
            if (ImGui::BeginListBox("##keylist", ImVec2(-FLT_MIN, -FLT_MIN)))
            {
                for (int k = static_cast<int>(sf::Keyboard::Key::A); k <= static_cast<int>(sf::Keyboard::KeyCount); k++)
                {
                    sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(k);
                    if (m_KeyBindings.find(key) != m_KeyBindings.end() && key != editKey)
                        continue;

                    std::string keyName = sf::Keyboard::getDescription(static_cast<sf::Keyboard::Scancode>(key)).toAnsiString();
                    ImGui::PushID(keyName.c_str());
                    const bool is_selected = (editKey == key);
                    if (ImGui::Selectable(keyName.c_str(), is_selected))
                    {
                        editKey = key;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
            ImGui::EndChild();
            
            ImGui::SameLine();
            
			// display available actions
			AvailableActionsList(editAction, "##actionlist");

            if (submitKeyError)
            {
                ErrorMessage("Please select a key and an action before submitting.");
            }
            
            if(ImGui::Button("Submit"))
            {
                if (editKey == sf::Keyboard::Key::Unknown || editAction == 0)
                {
                    submitKeyError = true;
                }
                else 
                {
                    submitKeyError = false;
                    m_KeyBindings[editKey] = ActionType{ editAction };
                    editAction = 0;
                    editKey = sf::Keyboard::Key::Unknown;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }

		// MOUSE BINDINGS 

        // mouse binding ui vars
		static std::optional<sf::Mouse::Button> currentMouseButton; // Currently selected mouse button
        static std::optional<sf::Mouse::Button> editMouseButton;    // Mouse button to replace binding
		static uint32_t editMouseAction = 0;                        // Action ID to replace binding
		static bool submitMouseButtonError = false;                        // Error flag for submitting mouse button binding

		// Mouse button bindings
        ImGui::SeparatorText("Mouse Button Bindings");
        if(ImGui::BeginListBox("##Mouse Button Bindings"))
        {
            for(auto& [button, action] : m_MouseButtonBindings)
            {   
                std::string buttonName = InputCompUtil::MouseButtonToString(button);
                ImGui::PushID(buttonName.c_str());
				const bool is_selected = currentMouseButton == button;
                if(ImGui::Selectable(buttonName.c_str(), is_selected))
                {
                    currentMouseButton = button;
                }
                if(is_selected)
                    ImGui::SetItemDefaultFocus();
                ImGui::PopID();
            }
            ImGui::EndListBox();
        }
		ImGui::SameLine(); HelpMarker("Map a mouse button to a registered event.");

		// add/edit mouse button bindings
        ImGui::SeparatorText("Mouse Button Action Mapping");
        if(currentMouseButton.has_value() && m_MouseButtonBindings.count(currentMouseButton.value()))
        {
            const char* editMouseWindow = "Edit Mouse Binding";
            std::string buttonName = InputCompUtil::MouseButtonToString(currentMouseButton.value());
            ImGui::Text("Mouse Button: %s", buttonName.c_str());
            ImGui::Text("Action: %s", ActionRegistry::Get().GetName(m_MouseButtonBindings[currentMouseButton.value()]).c_str());
            
            if (ImGui::Button("Edit Mouse Binding"))
            {
				editMouseButton = currentMouseButton;
				editMouseAction = m_MouseButtonBindings[currentMouseButton.value()].m_ID;
                ImGui::OpenPopup(editMouseWindow);
            } ImGui::SameLine();
            if (ImGui::Button("Remove Mouse Binding"))
            {
                m_MouseButtonBindings.erase(currentMouseButton.value());
                currentMouseButton.reset();
			}

            if(ImGui::BeginPopup(editMouseWindow))
            {   
                ImGui::BeginChild("Available Mouse Buttons", ImVec2(200, 300), true);
                if (ImGui::BeginListBox("##mousebuttonlist", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for (int b = static_cast<int>(sf::Mouse::Button::Left); b <= static_cast<int>(sf::Mouse::Button::Extra2); b++)
                    {
                        sf::Mouse::Button button = static_cast<sf::Mouse::Button>(b);
                        if (m_MouseButtonBindings.find(button) != m_MouseButtonBindings.end() && button != editMouseButton)
                            continue;
                        std::string buttonName = InputCompUtil::MouseButtonToString(button);
                        ImGui::PushID(buttonName.c_str());
                        const bool is_selected = (editMouseButton == button);
                        if (ImGui::Selectable(buttonName.c_str(), is_selected))
                        {
                            editMouseButton = button;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();
                
                ImGui::SameLine();
                
				// display available actions
				AvailableActionsList(editMouseAction, "##mouseactionlist");

                if (submitMouseButtonError)
                {
                    ErrorMessage("Please select a valid mouse button before submitting.");
				}
                
                if(ImGui::Button("Submit"))
                {
                    if (editMouseButton.has_value())
                    {
						submitMouseButtonError = false;
                        m_MouseButtonBindings.erase(currentMouseButton.value());
                        m_MouseButtonBindings[editMouseButton.value()] = ActionType{ editMouseAction };
                        currentMouseButton.reset();
                        editMouseButton.reset();
                        editMouseAction = 0;
                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        submitMouseButtonError = true;
					}
                }
                ImGui::SameLine();
                if(ImGui::Button("Cancel"))
                {
                    editMouseAction = 0;
                    editMouseButton.reset();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        else
        {
            ImGui::TextDisabled("No mapping available");
        }
        const char* newMouseBindWindow = "New Mouse Binding";
        if (ImGui::Button("Add New Mouse Binding"))
            ImGui::OpenPopup(newMouseBindWindow);
        if (ImGui::BeginPopup(newMouseBindWindow))
        {
            ImGui::BeginChild("Available Mouse Buttons", ImVec2(200, 300), true);
            if (ImGui::BeginListBox("##mousebuttonlist", ImVec2(-FLT_MIN, -FLT_MIN)))
            {
                for (int b = static_cast<int>(sf::Mouse::Button::Left); b <= static_cast<int>(sf::Mouse::Button::Extra2); b++)
                {
                    sf::Mouse::Button button = static_cast<sf::Mouse::Button>(b);
                    if (m_MouseButtonBindings.find(button) != m_MouseButtonBindings.end() && button != editMouseButton)
                        continue;
                    std::string buttonName = InputCompUtil::MouseButtonToString(button);
                    ImGui::PushID(buttonName.c_str());
                    const bool is_selected = (editMouseButton == button);
                    if (ImGui::Selectable(buttonName.c_str(), is_selected))
                    {
                        editMouseButton = button;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
            ImGui::EndChild();

            ImGui::SameLine();

            // display available actions
            AvailableActionsList(editMouseAction, "##mouseactionlist");

            if (submitMouseButtonError)
            {
                ErrorMessage("Please select a valid mouse button before submitting.");
            }

            if (ImGui::Button("Submit"))
            {
				if (editMouseButton.has_value())
                { 
					submitMouseButtonError = false;
                    m_MouseButtonBindings[editMouseButton.value()] = ActionType{ editMouseAction };
                    editMouseAction = 0;
                    editMouseButton.reset();
                    ImGui::CloseCurrentPopup();
                }
                else {
                    submitMouseButtonError = true;
                }
            }
            ImGui::EndPopup();
        }
    }
}
