#pragma once

#include "nlohmann/json.hpp"

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

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpriteCords, x, y, width, height)
    
    struct AnimationData
    {
        std::string ID;
        std::string textureID;
        std::vector<SpriteCords> spriteCords;
        float frameRate = 0.1f;
        bool isLooping = true;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationData, ID, textureID, spriteCords)
}