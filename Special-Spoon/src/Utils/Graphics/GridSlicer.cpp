#include "GridSlicer.h"

namespace Spoon
{
    bool AtlasGrid::IsValid() const
    {
        return cell.x > 0 && cell.y > 0 && size.x > 0 && size.y > 0 &&
               margin >= 0 && spacing >= 0;
    }

    int AtlasGrid::Count() const
    {
        return IsValid() ? size.x * size.y : 0;
    }

    std::optional<sf::IntRect> AtlasGrid::RectForIndex(int index) const
    {
        if (!IsValid() || index < 0 || index >= Count())
            return std::nullopt;

        const int c = index % size.x;
        const int r = index / size.x;

        const int x = margin + c * (cell.x + spacing);
        const int y = margin + r * (cell.y + spacing);

        return sf::IntRect({x, y}, {cell.x, cell.y});
    }

    std::optional<int> AtlasGrid::IndexAtPixel(sf::Vector2i p) const
    {
        if (!IsValid())
            return std::nullopt;

        const int x = p.x - margin;
        const int y = p.y - margin;
        if (x < 0 || y < 0)
            return std::nullopt;

        const int stepX = cell.x + spacing;
        const int stepY = cell.y + spacing;
        if (stepX <= 0 || stepY <= 0)
            return std::nullopt;

        const int c = x / stepX;
        const int r = y / stepY;
        if (c < 0 || c >= size.x || r < 0 || r >= size.y)
            return std::nullopt;

        // reject spacing gaps
        if ((x % stepX) >= cell.x || (y % stepY) >= cell.y)
            return std::nullopt;

        return r * size.x + c;
    }
}