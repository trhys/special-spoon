cmake_minimum_required(VERSION 3.15...4.0)

project(special-spoon
    VERSION 0.1
    LANGUAGE CXX
)

add_executable(Sandbox)

target_sources(Sandbox
    PRIVATE
        Sandbox.cpp
        DemoLayer.cpp
        DemoZombie.cpp

    PUBLIC
        FILE_SET sandbox_headers
        TYPE HEADERS
        FILES
            Background.h
            DemoLayer.h
            DemoZombie.h
)

add_library(special-spoon-core)

target_sources(special-spoon-core)
    PRIVATE
        Application.cpp
        Layer.cpp
        LayerStack.cpp
        Scene/Node.cpp
        Physics/CollisionDetector.cpp
        Physics/QuadTree.cpp

    PUBLIC
        FILE_SET ssc_headers
        TYPE HEADERS
        FILES
            Application.h
            Core.h
            EntryPoint.h
            Layer.h
            LayerStack.h
            MemoryUtils.h
            ResourceManager.h
            Spoon.h
            Scene/Entity.h
            Scene/Node.h
            Scene/Scene.h
            Physics/CollisionDetector.h
            Physics/PhysicsManager.h
            Physics/QuadTree.h

target_link_libraries(Sandbox PRIVATE special-spoon-core)