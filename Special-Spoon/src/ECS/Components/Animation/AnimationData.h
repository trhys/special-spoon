#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <vector>
#include <string>

namespace Spoon
{
    struct SpriteCords
    {
        SpriteCords() = default;
        SpriteCords(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
        int x;
        int y;
        int width;
        int height;
    };
    
    struct AnimationData
    {
        std::string ID;
        std::string textureID;
        std::vector<SpriteCords> spriteCords;
        float frameRate = 0.1f;
        bool isLooping = true;
    };

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
            {"textureID", v.textureID},
            {"spriteCords", v.spriteCords},
            {"frameRate", v.frameRate},
            {"isLooping", v.isLooping}
        };
    }

    inline void from_json(const json& j, AnimationData& v)
    {
        v.ID = j.at("ID").get<std::string>();
        v.textureID = j.at("textureID").get<std::string>();
        v.spriteCords = j.at("spriteCords").get<std::vector<SpriteCords>>();
        v.frameRate = j.value("frameRate", 0.1f);
        v.isLooping = j.value("isLooping", true);
    }
}