#pragma once

#include "Core/Core.h"
#include "Core/EntityManager/EntityManager.h"
#include "Core/Renderer/WorldProjection.h"

#include "SFML/System/Time.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderStates.hpp"


namespace Spoon
{
    // scheduler edge ording interface
    struct SystemAccess
    {
        std::vector<std::string> reads;
        std::vector<std::string> writes;
    };

    class ISystem
    {
    public:
        ISystem(std::string name) : m_DisplayName(name) {}
        virtual ~ISystem() {}

        virtual void Update(sf::Time tick, EntityManager& manager) = 0;
        virtual void OnReflect() {}
        virtual json Serialize() { return nullptr; }

        std::string m_DisplayName;
        std::string& GetDisplayName() { return m_DisplayName; }

        virtual std::vector<std::string> RunAfter()  const { return {}; }
        virtual std::vector<std::string> RunBefore() const { return {}; }
        virtual SystemAccess GetAccess() const { return {}; }

        // overlay rendering function - returns number of draws
        virtual int PushOverlay(
            sf::RenderTarget& target, 
            sf::RenderStates states, 
            const WorldProjection& projection
        ) { return 0; }
    };
}