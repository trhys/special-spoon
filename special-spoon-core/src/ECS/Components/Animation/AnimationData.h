#pragma once

#include <vector>

namespace Spoon
{
    struct SpriteCords
    {
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
}