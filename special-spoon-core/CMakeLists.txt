add_library(special-spoon-core SHARED)

add_definitions(-DSS_BUILD_DLL)

target_sources(special-spoon-core
    PRIVATE
        Application.cpp
        ECS/Node/Node.cpp
        ECS/Component.cpp
        ECS/TextComp.cpp
        Layer.cpp
        LayerStack.cpp
        Scene/ResourceManager.cpp
        Scene/Scene.cpp
        Scene/SceneManager.cpp
        Physics/CollisionDetector.cpp
        Physics/QuadTree.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            Application.h
            Core.h
            EntryPoint.h
            Layer.h
            LayerStack.h                     
            ECS/Component.h
            ECS/PhysComp.h
            ECS/SpriteComp.h
            ECS/TextComp.h
            ECS/TransComp.h
            ECS/Node/Entity.h
            ECS/Node/Node.h
            Scene/Scene.h
            Scene/SceneManager.h
            Scene/ResourceManager.h
            Spoon.h
            Physics/CollisionDetector.h
            Physics/PhysicsManager.h
            Physics/QuadTree.h
            Utils/MemoryUtils.h  
)

find_package(SFML REQUIRED system audio network window graphics)

target_link_libraries(special-spoon-core PUBLIC ${SFML_LIBRARIES})

include(GNUInstallDirs)

install(
    TARGETS special-spoon-core
    EXPORT special-spoon
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/special-spoon
    FILE_SET HEADERS
        DESTINATION DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/special-spoon
)

install(
    EXPORT special-spoon-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/special-spoon
    NAMESPACE special-spoon::
)