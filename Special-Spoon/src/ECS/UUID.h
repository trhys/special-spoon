#pragma once

#include <cstdint>
#include <string>

namespace Spoon
{
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