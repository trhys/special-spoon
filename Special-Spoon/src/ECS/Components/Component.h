#pragma once

#include "Core/Core.h"
#include "Imgui/imgui.h"

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

        std::string& GetDisplayName()
        {
            return m_Name;
        }

        std::string m_Name;
    };
}