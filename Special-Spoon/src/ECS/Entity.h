#pragma once

#include "Core/Core.h"
#include "ECS/Components/Component.h"

#include <cstdint>
#include <string>

namespace Spoon
{   
    class EntityManager;

    struct UUID
    {
        UUID() :ID(0) {}
        UUID(std::uint64_t id) : ID(id) {}
        std::uint64_t ID;

        bool operator==(const UUID& other) const { return ID == other.ID; }
        bool operator!=(const UUID& other) const { return ID != other.ID; }
        bool operator<(const UUID& other) const { return ID < other.ID; }
        bool operator>(const UUID& other) const { return ID > other.ID; }

        // Serialization helpers
        std::string ToString() const { return std::to_string(ID); }
        static std::uint64_t ToID(std::string str) { return std::stoull(str); }
    };

    class SPOON_API Entity
    {
    public:
        Entity() {}
        Entity(UUID id, EntityManager* em) : m_ID(id), p_EM(em) {}
        ~Entity() = default;
        
        template<typename COMP, typename... Args>
        void AddComponent(Args&&... args);

        template<typename COMP>
        void DropComponent();

        UUID GetID() const { return m_ID; }

    private:
        EntityManager* p_EM;
        UUID m_ID;
    };
}

namespace std
{
    template <>
    struct hash<Spoon::UUID>
    {
        std::size_t operator()(const Spoon::UUID& uuid) const noexcept
        {
            return std::hash<std::uint64_t>{}(uuid.ID);
        }
    };
}