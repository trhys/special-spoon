#pragma once

#include "ECS/Entity.h"
#include "ECS/Components/Animation/AnimationData.h"
#include "Core/Registers/ActionRegistry.h"

#include "nlohmann/json.hpp"
#include "SFML/Graphics.hpp"

using json = nlohmann::json;

namespace sf
{
    inline void to_json(json& j, const Vector2f& v)
    {
        j = json{{"X", v.x}, {"Y", v.y}};
    }

    inline void from_json(const json& j, Vector2f& v)
    {
        v.x = j.at("X").get<float>();
        v.y = j.at("Y").get<float>();
    }

    inline void to_json(json& j, const Vector2i& v)
    {
        j = json{{"X", v.x}, {"Y", v.y}};
    }

    inline void from_json(const json& j, Vector2i& v)
    {
        v.x = j.at("X").get<int>();
        v.y = j.at("Y").get<int>();
    }

    inline void to_json(json& j, const IntRect& r)
    {
        j = json{
            {"left", r.position.x}, 
            {"top", r.position.y}, 
            {"width", r.size.x}, 
            {"height", r.size.y}
        };
    }

    inline void from_json(const json& j, IntRect& r)
    {
        j.at("left").get_to(r.position.x);
        j.at("top").get_to(r.position.y);
        j.at("width").get_to(r.size.x);
        j.at("height").get_to(r.size.y);
    }

    inline void to_json(json& j, const Color& v)
    {
        j = json{{"R", v.r}, {"G", v.g}, {"B", v.b}, {"A", v.a}};
    }

    inline void from_json(const json& j, Color& v)
    {
        v.r = static_cast<std::uint8_t>(j.at("R").get<int>());
        v.g = static_cast<std::uint8_t>(j.at("G").get<int>());
        v.b = static_cast<std::uint8_t>(j.at("B").get<int>());
        v.a = static_cast<std::uint8_t>(j.at("A").get<int>());
    }
}

namespace Spoon
{
    struct UUID;
    
    inline void to_json(json& j, const UUID& v)
    {
        j = v.ToString();
    }

    inline void from_json(const json& j, UUID& v)
    {
        v.ID = UUID::ToID(j.get<std::string>());
    }

    inline void to_json(json& j, const SpriteCords& v)
    {
        j = json{
            { "x", v.x },
            { "y", v.y },
            { "width", v.width },
            { "height", v.height }
        };
    }

    inline void from_json(const json& j, SpriteCords& v)
    {
        v.x = j["x"].get<int>();
        v.y = j["y"].get<int>();
        v.width = j["width"].get<int>();
        v.height = j["height"].get<int>();
    }

    inline void to_json(json& j, const AnimationData& v)
    {
        j = json{
            {"ID", v.ID},
            {"TextureID", v.textureID},
            {"SpriteCords", v.spriteCords}
        };
    }

    inline void from_json(const json& j, AnimationData& v)
    {
        v.ID = j.at("ID").get<std::string>();
        v.textureID = j.at("TextureID").get<std::string>();
        v.spriteCords = j.at("SpriteCords").get<std::vector<SpriteCords>>();
    }

    inline void to_json(json& j, const ActionType& v)
    {
        j = v.m_ID;
	}

    inline void from_json(const json& j, ActionType& v)
    {
        v.m_ID = j.get<uint32_t>();
	}
}