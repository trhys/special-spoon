#include "QuadTree.h"

#include <optional>

namespace Spoon
{
    void Quadtree::BuildTree(sf::Vector2f gridSize)
    {
        sf::Vector2f node_size = { gridSize.x / 4, gridSize.y / 2 };
        for (auto& leaf : m_GridNodes)
        {
            leaf.body.size = node_size;
            leaf.rect.setSize(node_size);
            leaf.rect.setOutlineThickness(1.0f);
            leaf.rect.setFillColor(sf::Color::Transparent);
        }

        for (size_t in = 0; in < m_GridNodes.size(); in++)
        {
            if (in < 4)
            {
                m_GridNodes[in].body.position.x = node_size.x * static_cast<float>(in);
                m_GridNodes[in].body.position.y = 0.0f;
                m_GridNodes[in].rect.setPosition({ node_size.x * static_cast<float>(in), 0.0f });
            }
            else
            {
                int bottom_index = static_cast<int>(in) - 4;
                m_GridNodes[in].body.position.x = node_size.x * static_cast<float>(bottom_index);
                m_GridNodes[in].body.position.y = node_size.y;
                m_GridNodes[in].rect.setPosition({ node_size.x * static_cast<float>(bottom_index), node_size.y });
            }
        }
    }

    void Quadtree::Populate(const std::unordered_map<UUID, sf::FloatRect>& bounds)
    {
        for (auto& leaf : m_GridNodes)
            leaf.collision_buffer.clear();

        for (const auto& [entity, entityBox] : bounds)
        {
            for (auto& leaf : m_GridNodes)
            {
                if (const std::optional intersect = leaf.body.findIntersection(entityBox))
                {
                    leaf.collision_buffer.push_back(entity);
                }
            }
        }
    }

    std::set<std::pair<UUID, UUID>> Quadtree::GeneratePairs()
    {
        std::set<std::pair<UUID, UUID>> unique_pairs;
        for (auto& leaf : m_GridNodes)
        {
            for (size_t a = 0; a < leaf.collision_buffer.size(); a++)
            {
                for (size_t b = a + 1; b < leaf.collision_buffer.size(); b++)
                {
                    UUID entityA = leaf.collision_buffer[a];
                    UUID entityB = leaf.collision_buffer[b];
                    if (entityA > entityB)
                        std::swap(entityA, entityB);
                    unique_pairs.insert({ entityA, entityB });
                }
            }
        }
        return unique_pairs;
    }
}
