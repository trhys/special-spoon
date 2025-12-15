target_sources(special-spoon-core
    PRIVATE
        AnimationSystem.h
        InputSystem.h
        StateSystem.h
        System.h
        SystemLoaders.h
        SystemManager.h

        Physics/PhysicsSystem.h
        Physics/QuadTree.h    

        # IMPL ====================
        
        Physics/QuadTree.cpp

        AnimationSystem.cpp
        SystemLoaders.cpp
)