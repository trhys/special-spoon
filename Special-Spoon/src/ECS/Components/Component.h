#pragma once

#include "Core/Core.h"
#include "Core/Serialization/JsonTypeDefs.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

#include <string>

namespace Spoon
{
    class Component
    {
    public:
        Component() {}
        virtual ~Component() {}

        virtual void OnReflect() {}
        virtual const char* GetType() = 0;
        virtual std::string& GetDisplayName() = 0;
        virtual json Serialize() = 0;

        virtual bool ActiveGizmo() = 0;
        virtual void ToggleGizmo() = 0;
    };

    template<typename COMP>
    struct ComponentBase : public Component
    {
        ComponentBase(std::string id) : m_Name(id) {}
        virtual ~ComponentBase() {}

        const char* GetType() override
        {
            return typeid(COMP).name();
        }

        std::string& GetDisplayName() override
        {
            return m_Name;
        }

        json Serialize() override
        {
            json j;
            nlohmann::to_json(j, *static_cast<COMP*>(this));
            return j;
        }

        bool ActiveGizmo() override
        {
            return m_gizmoActive;
        }

        void ToggleGizmo() override
        {
            m_gizmoActive = !m_gizmoActive;
        }

        std::string m_Name;
        bool m_gizmoActive = false;
    };
}