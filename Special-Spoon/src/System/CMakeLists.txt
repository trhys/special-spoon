target_sources(Special-Spoon
    PRIVATE
        Include.h  
        System.h
        SystemManager.h 

        Core/InputSystem.h
        Core/InputSystem.cpp
        Core/StateSystem.h
        Core/StateSystem.cpp

        Physics/CollisionSystem.h
        Physics/CollisionSystem.cpp
        Physics/MovementSystem.h
        Physics/MovementSystem.cpp
        Physics/PhysicsSystem.h
        Physics/SystemConfigs.h
        Physics/QuadTree.h            
        Physics/QuadTree.cpp

        Render/AnimationSystem.h
        Render/AnimationSystem.cpp
        Render/ParticleSystem.h
        Render/ParticleSystem.cpp
        Render/UISystem.h
        Render/UISystem.cpp
        Render/UICursor.h

        TileMap/TileMapCollisionSystem.h
        TileMap/TileMapCollisionSystem.cpp

        Sound/AudioSystem.h
        Sound/AudioSystem.cpp
)
