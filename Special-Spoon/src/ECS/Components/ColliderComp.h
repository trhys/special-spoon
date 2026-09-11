#pragma once

#include "ECS/Components/Component.h"
#include "ECS/UUID.h"

#include <memory>
#include <string>
#include <vector>

namespace Spoon
{
    enum class ColliderType
    {
        AABB = 0,
        Circle = 1
    };

    struct ICollider
    {
        virtual ~ICollider() = default;
        virtual ColliderType GetType() const = 0;
        virtual sf::FloatRect GetBounds(const sf::Vector2f& worldOrigin) const = 0;
        virtual std::unique_ptr<ICollider> Clone() const = 0;
    };

    struct AABBCollider : public ICollider
    {
        AABBCollider() = default;
        explicit AABBCollider(sf::Vector2f inSize) : size(inSize) {}

        sf::Vector2f size = { 32.0f, 32.0f };

        ColliderType GetType() const override { return ColliderType::AABB; }
        sf::FloatRect GetBounds(const sf::Vector2f& worldOrigin) const override
        {
            return sf::FloatRect(worldOrigin, size);
        }

        std::unique_ptr<ICollider> Clone() const override
        {
            return std::make_unique<AABBCollider>(*this);
        }
    };

    struct CircleCollider : public ICollider
    {
        CircleCollider() = default;
        explicit CircleCollider(float inRadius) : radius(inRadius) {}

        float radius = 16.0f;

        ColliderType GetType() const override { return ColliderType::Circle; }
        sf::FloatRect GetBounds(const sf::Vector2f& worldOrigin) const override
        {
            return sf::FloatRect(
                sf::Vector2f(worldOrigin.x - radius, worldOrigin.y - radius),
                sf::Vector2f(radius * 2.0f, radius * 2.0f)
            );
        }

        std::unique_ptr<ICollider> Clone() const override
        {
            return std::make_unique<CircleCollider>(*this);
        }
    };

    struct ColliderComp : public ComponentBase<ColliderComp>
    {
        ColliderComp() : ComponentBase::ComponentBase(Name), m_Collider(std::make_unique<AABBCollider>()) {}

        ColliderComp(const ColliderComp& other) : ComponentBase::ComponentBase(Name)
        {
            offset = other.offset;
            touchingThisFrame = other.touchingThisFrame;
            touchingLastFrame = other.touchingLastFrame;
            if (other.m_Collider)
                m_Collider = other.m_Collider->Clone();
        }

        ColliderComp& operator=(const ColliderComp& other)
        {
            if (this == &other)
                return *this;

            offset = other.offset;
            touchingThisFrame = other.touchingThisFrame;
            touchingLastFrame = other.touchingLastFrame;
            m_Collider.reset();
            if (other.m_Collider)
                m_Collider = other.m_Collider->Clone();
            return *this;
        }

        ColliderComp(ColliderComp&&) noexcept = default;
        ColliderComp& operator=(ColliderComp&&) noexcept = default;

        static constexpr const char* Name = "Collider";

        void SetAABBSize(const sf::Vector2f& size)
        {
            m_Collider = std::make_unique<AABBCollider>(size);
        }

        void SetCircleRadius(float radius)
        {
            m_Collider = std::make_unique<CircleCollider>(radius);
        }

        ColliderType GetType() const
        {
            return m_Collider ? m_Collider->GetType() : ColliderType::AABB;
        }

        sf::FloatRect GetWorldBounds(const sf::Vector2f& transformPosition) const
        {
            if (!m_Collider)
                return sf::FloatRect();
            return m_Collider->GetBounds(transformPosition + offset);
        }

        sf::Vector2f GetWorldOrigin(const sf::Vector2f& transformPosition) const
        {
            return transformPosition + offset;
        }

        const AABBCollider* AsAABB() const
        {
            return GetType() == ColliderType::AABB ? static_cast<const AABBCollider*>(m_Collider.get()) : nullptr;
        }

        const CircleCollider* AsCircle() const
        {
            return GetType() == ColliderType::Circle ? static_cast<const CircleCollider*>(m_Collider.get()) : nullptr;
        }

        void OnReflect() override
        {
            const char* shapeTypes[] = { "AABB", "Circle" };
            int typeIndex = static_cast<int>(GetType());
            if (ImGui::Combo("Shape##collider", &typeIndex, shapeTypes, 2))
            {
                if (typeIndex == static_cast<int>(ColliderType::AABB))
                    SetAABBSize({ 32.0f, 32.0f });
                else
                    SetCircleRadius(16.0f);
            }

            if (GetType() == ColliderType::AABB)
            {
                auto* collider = static_cast<AABBCollider*>(m_Collider.get());
                ImGui::SliderFloat("Width##collider", &collider->size.x, 0.0f, 500.0f, "%.2f");
                ImGui::SliderFloat("Height##collider", &collider->size.y, 0.0f, 500.0f, "%.2f");
            }
            else if (GetType() == ColliderType::Circle)
            {
                auto* collider = static_cast<CircleCollider*>(m_Collider.get());
                ImGui::SliderFloat("Radius##collider", &collider->radius, 0.0f, 500.0f, "%.2f");
            }

            ImGui::SliderFloat("Offset X##collider", &offset.x, -500.0f, 500.0f, "%.2f");
            ImGui::SliderFloat("Offset Y##collider", &offset.y, -500.0f, 500.0f, "%.2f");
            ImGui::Text("Touching this frame: %zu", touchingThisFrame.size());
            ImGui::Text("Touching last frame: %zu", touchingLastFrame.size());
        }

        sf::Vector2f offset = { 0.0f, 0.0f };
        std::vector<UUID> touchingThisFrame;
        std::vector<UUID> touchingLastFrame;
        std::unique_ptr<ICollider> m_Collider;
    };

    inline void to_json(json& j, const ColliderComp& comp)
    {
        j = json{
            {"offset", comp.offset}
        };

        json colliderData;
        if (comp.GetType() == ColliderType::Circle)
        {
            const auto* circle = comp.AsCircle();
            colliderData = json{
                {"type", "Circle"},
                {"radius", circle ? circle->radius : 16.0f}
            };
        }
        else
        {
            const auto* box = comp.AsAABB();
            colliderData = json{
                {"type", "AABB"},
                {"size", box ? box->size : sf::Vector2f{ 32.0f, 32.0f }}
            };
        }

        j["collider"] = colliderData;
    }

    inline void from_json(const json& j, ColliderComp& comp)
    {
        if (j.contains("offset"))
            comp.offset = j.at("offset").get<sf::Vector2f>();

        if (j.contains("collider"))
        {
            const auto& collider = j.at("collider");
            const std::string type = collider.value("type", "AABB");
            if (type == "Circle")
            {
                comp.SetCircleRadius(collider.value("radius", 16.0f));
            }
            else
            {
                if (collider.contains("size"))
                    comp.SetAABBSize(collider.at("size").get<sf::Vector2f>());
                else
                    comp.SetAABBSize({ collider.value("width", 32.0f), collider.value("height", 32.0f) });
            }
        }
        else
        {
            comp.SetAABBSize({ 32.0f, 32.0f });
        }
    }
}
