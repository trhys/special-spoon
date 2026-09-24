target_sources(Special-Spoon
    PRIVATE
        AnimationSystem.h
        AnimationSystem.cpp
        InputSystem.h
        InputSystem.cpp
        MovementSystem.h
        StateSystem.h
        StateSystem.cpp
        System.h
        SystemManager.h

        Physics/CollisionSystem.h
        Physics/CollisionSystem.cpp
        Physics/PhysicsSystem.h
        Physics/SystemConfigs.h
        Physics/QuadTree.h            
        Physics/QuadTree.cpp

        TileMap/TileMapCollisionSystem.h
        TileMap/TileMapCollisionSystem.cpp
)
