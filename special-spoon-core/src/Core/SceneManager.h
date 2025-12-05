#pragma once

#include "Core.h"
#include <unordered_map>

namespace Spoon
{
    struct FilePaths
    {
        std::string ID;
        std::string ResourceFiles;
        std::string DataFiles;
    };

    class SPOON_API SceneManager
    {
    public:
        SceneManager() {}
        ~SceneManager() {}

        void LoadManifest(std::string manifestPath);
        void LoadScene(std::string id);

    private:
        std::unordered_map<std::string, FilePaths> m_SceneManifest;
    };
}