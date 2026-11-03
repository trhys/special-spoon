#pragma once

#include "nlohmann/json.hpp"
#include <SFML/Graphics.hpp>
#include <optional>

namespace Spoon
{
    struct AtlasGrid
    {
        sf::Vector2i cell{16,16};   // size of cell in px
        sf::Vector2i size{1,1};     // x=columns, y=rows
        int margin = 0;
        int spacing = 0;

        bool IsValid() const;
        int Count() const;
        std::optional<sf::IntRect> RectForIndex(int index) const; // 0-based
        std::optional<int> IndexAtPixel(sf::Vector2i p) const;    // 0-based, ignores gaps
    };
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AtlasGrid, cell, size, margin, spacing)
}