target_sources(Special-Spoon
    PRIVATE
        AnimationSystem.h
        InputSystem.h
        InputSystem.cpp
        StateSystem.h
        System.h
        SystemManager.h

        Physics/PhysicsSystem.h
        Physics/QuadTree.h    

        # IMPL ====================
        
        Physics/QuadTree.cpp

        AnimationSystem.cpp
)