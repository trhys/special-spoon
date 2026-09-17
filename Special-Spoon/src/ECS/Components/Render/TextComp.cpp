#include "ECS/Components/Render/TextComp.h"

namespace Spoon {
  void TextComp::CenterOrigin() 
  {
      if (isCentered)
      {
          sf::FloatRect bounds = m_Text.getLocalBounds(); 
          m_Text.setOrigin({ 
              bounds.position.x + (bounds.size.x / 2.0f), 
              bounds.position.y + (bounds.size.y / 2.0f) 
          });
      }
      else
      {
          m_Text.setOrigin({0.0, 0.0});
      }
      
  }

  void TextComp::SetAlpha(float alpha)
  {
      sf::Color color = m_Text.getFillColor();
      color.a = static_cast<uint8_t>(alpha);
      m_Text.setFillColor(color);
  }

  void TextComp::OnReflect()
  {
      std::string text = m_Text.getString();
      static char newTextBuf[64];
      
      ImGui::Text("Text: %s", text.c_str());
      if (ImGui::Button("Edit String"))
      {
          strncpy(newTextBuf, text.c_str(), sizeof(newTextBuf));
          ImGui::OpenPopup("Edit Text String");
      }

      // Always center this window when appearing
      ImVec2 center = ImGui::GetMainViewport()->GetCenter();
      ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
      if (ImGui::BeginPopupModal("Edit Text String"))
      {
          bool changedString = ImGui::InputText("##New String", newTextBuf, IM_ARRAYSIZE(newTextBuf), ImGuiInputTextFlags_EnterReturnsTrue);
          
          if (ImGui::Button("Submit") || changedString)
          {
              iText = newTextBuf;
              m_Text.setString(newTextBuf);
              CenterOrigin();
              ImGui::CloseCurrentPopup();
          }
          ImGui::SameLine();
          if (ImGui::Button("Cancel"))
          {
              ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
      }

      if (ImGui::BeginChild("Font Explorer", ImVec2(0, 200), ImGuiChildFlags_Borders))
      {   
          for (const auto& [id, font] : ResourceManager::Get().GetFonts())
          {
              ImGui::PushID(id.c_str());
              ImGui::Image(ResourceManager::Get().GetFontPreview(id));
              ImGui::SameLine();
              if (ImGui::Selectable(id.c_str()))
              {
                  m_Text.setFont(font);
                  iFontID = id;
              }
              ImGui::PopID();
          }
          ImGui::EndChild();
      }

      int charSize = static_cast<int>(m_Text.getCharacterSize());
      ImGui::SeparatorText("Character Size");
      if (ImGui::SliderInt("##CharSize", &charSize, 1, 100))
      {
          iCharSize = charSize;
          SetSize(static_cast<unsigned int>(charSize));
          CenterOrigin();
      }

      ImGui::SeparatorText("Center Origin");
      if(ImGui::Checkbox("Centered", &isCentered)) CenterOrigin();

      ImGui::SeparatorText("Fill Color");
      sf::Color m_Color = m_Text.getFillColor();
      float color[4] = {
          m_Color.r / 255.0f,
          m_Color.g / 255.0f,
          m_Color.b / 255.0f,
          m_Color.a / 255.0f
      };
      if (ImGui::ColorEdit4("Fill Color", color))
      {
          m_Color.r = static_cast<std::uint8_t>(color[0] * 255.0f);
          m_Color.g = static_cast<std::uint8_t>(color[1] * 255.0f);
          m_Color.b = static_cast<std::uint8_t>(color[2] * 255.0f);
          m_Color.a = static_cast<std::uint8_t>(color[3] * 255.0f);
          iColor = m_Color;
          SetColor(m_Color);
      }

      ImGui::SeparatorText("Outline");
      sf::Color outline_Color = m_Text.getOutlineColor();
      float OLcolor[4] = {
          outline_Color.r / 255.0f,
          outline_Color.g / 255.0f,
          outline_Color.b / 255.0f,
          outline_Color.a / 255.0f
      };
      if (ImGui::ColorEdit4("Outline Color", OLcolor))
      {
          outline_Color.r = static_cast<std::uint8_t>(OLcolor[0] * 255.0f);
          outline_Color.g = static_cast<std::uint8_t>(OLcolor[1] * 255.0f);
          outline_Color.b = static_cast<std::uint8_t>(OLcolor[2] * 255.0f);
          outline_Color.a = static_cast<std::uint8_t>(OLcolor[3] * 255.0f);
          iOutColor = outline_Color;
          m_Text.setOutlineColor(outline_Color);
      }

      float olThickness = m_Text.getOutlineThickness();
      if (ImGui::SliderFloat("Outline Thickness", &olThickness, 0.0f, 50.0f))
      {
          iolThickness = olThickness;
          m_Text.setOutlineThickness(olThickness);
          CenterOrigin();
      }
  }

  void TextComp::PreRender(EntityManager& manager, UUID id)
  {
      auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
      auto& colorArray = manager.GetArray<ColorComp>(ColorComp::Name);
    
      if(transformArray.m_IdToIndex.count(id))
      {
          TransformComp& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
          m_Text.SetPosition(transform.GetPosition());
          m_Text.SetRotation(transform.m_Transform.getRotation().asDegrees());
      }
    
      if(colorArray.m_IdToIndex.count(id)) 
      {
          ColorComp& color = manager.GetComponent<ColorComp>(id, ColorComp::Name);
          m_Text.SetColor(color.m_Color);
      }
  }

  void TextComp::Render(sf::RenderTarget& target, sf::RenderStates states)
  {
      target.draw(m_Text, states);
  }
}
