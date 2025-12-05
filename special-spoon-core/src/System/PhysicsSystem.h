#pragma once

namespace Spoon
{
    class PhysicsSystem
    {
    public:
        PhysicsSystem() {}
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager);
        
    };
}