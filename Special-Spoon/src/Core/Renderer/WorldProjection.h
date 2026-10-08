#pragma once

#include "SFML/System/Vector2.hpp"

namespace Spoon
{
    // ==========================================================================
    // Coordinate-space contract
    // ==========================================================================
    //
    // Logical space (authoritative)
    //   - TransformComp positions, ColliderComp shapes/offsets, physics,
    //     tilemap collision and everything that is serialized.
    //   - One tilemap cell is `logicalCell` units (the atlas tile width/height),
    //     so cell (x, y) covers [x * w, (x + 1) * w) x [y * h, (y + 1) * h).
    //   - Gameplay code never stores presentation coordinates.
    //
    // Cell space
    //   - Fractional tile grid coordinates: cell = logical / logicalCell.
    //
    // Presentation space
    //   - The SFML world coordinates things are drawn in (and what
    //     `mapPixelToCoords` returns for the viewport).
    //   - When projection is active (gated by the project's Projection Style,
    //     see `UsesIsometricProjection()`) cells are projected into isometric
    //     diamonds of `projectedCell` size:
    //         screen.x = (cell.x - cell.y) * projectedCell.x / 2
    //         screen.y = (cell.x + cell.y) * projectedCell.y / 2
    //   - When projection is inactive, presentation == logical.
    //
    // Boundaries
    //   - Rendering projects logical -> presentation (tile vertices, sprites,
    //     debug collider outlines). Projected values are never written back.
    //   - Editor/runtime input inverse projects presentation -> logical before
    //     touching transforms or picking tile cells.
    // ==========================================================================
    struct WorldProjection
    {
        bool enabled = false;                       // project config gate
        sf::Vector2f logicalCell{ 16.0f, 16.0f };   // logical size of one tile cell
        sf::Vector2f projectedCell{ 16.0f, 16.0f }; // projected diamond footprint of one cell

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
