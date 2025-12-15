#pragma once

namespace Spoon
{
    class EntityManager;
    
    class Editor
    {
    public:
        Editor() {}
        ~Editor() {}

        void Run(EntityManager& manager);
        void Close();
    };
}