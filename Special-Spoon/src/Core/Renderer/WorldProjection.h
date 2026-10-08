#pragma once

#include "SFML/System/Vector2.hpp"

namespace Spoon
{
    // centralized helper for managing logical vs projected world coordinates
    // artistically, the world can be projected into an isometric view
    // but we maintain a separate logical grid system for the engine
    // to run it's simulation on. only the on screen presentation is projected.
    struct WorldProjection
    {
        bool enabled = false;                       
        sf::Vector2f logicalCell{ 16.0f, 16.0f };   
        sf::Vector2f projectedCell{ 16.0f, 16.0f };

        bool IsActive() const
        {
            return enabled &&
                logicalCell.x > 0.0f && logicalCell.y > 0.0f &&
                projectedCell.x > 0.0f && projectedCell.y > 0.0f;
        }

        // logical <-> cell
        sf::Vector2f LogicalToCell(const sf::Vector2f& logical) const
        {
            if (logicalCell.x <= 0.0f || logicalCell.y <= 0.0f)
                return { 0.0f, 0.0f };
            return { logical.x / logicalCell.x, logical.y / logicalCell.y };
        }

        sf::Vector2f CellToLogical(const sf::Vector2f& cell) const
        {
            return { cell.x * logicalCell.x, cell.y * logicalCell.y };
        }

        // cell <-> presentation
        sf::Vector2f CellToScreen(const sf::Vector2f& cell) const
        {
            if (!IsActive())
                return CellToLogical(cell);

            return {
                (cell.x - cell.y) * projectedCell.x * 0.5f,
                (cell.x + cell.y) * projectedCell.y * 0.5f
            };
        }

        sf::Vector2f ScreenToCell(const sf::Vector2f& screen) const
        {
            if (!IsActive())
                return LogicalToCell(screen);

            return {
                screen.x / projectedCell.x + screen.y / projectedCell.y,
                screen.y / projectedCell.y - screen.x / projectedCell.x
            };
        }

        // logical <-> presentation
        sf::Vector2f LogicalToScreen(const sf::Vector2f& logical) const
        {
            if (!IsActive())
                return logical;
            return CellToScreen(LogicalToCell(logical));
        }

        sf::Vector2f ScreenToLogical(const sf::Vector2f& screen) const
        {
            if (!IsActive())
                return screen;
            return CellToLogical(ScreenToCell(screen));
        }
    };
}
