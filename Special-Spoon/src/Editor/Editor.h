#pragma once

namespace Spoon
{
    class EntityManager;
    class SceneManager;
    class SystemManager;
    
    class Editor
    {
    public:
        Editor() {}
        ~Editor() {}

        void Run(EntityManager& manager, SceneManager& s_Manager, SystemManager& sys_Manager);
        void Close();
    };
}