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

    template <>
    struct hash<std::pair<Spoon::UUID, Spoon::UUID>>
    {
        std::size_t operator()(const std::pair<Spoon::UUID, Spoon::UUID>& p) const noexcept
        {
            std::size_t h1 = std::hash<Spoon::UUID>{}(p.first);
            std::size_t h2 = std::hash<Spoon::UUID>{}(p.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
}
