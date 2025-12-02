add_library(special-spoon-core SHARED)

add_definitions(-DSS_BUILD_DLL)

target_sources(special-spoon-core
    PRIVATE
        src/Application.cpp
        src/ECS/Node/Node.cpp
        src/ECS/Component.cpp
        src/ECS/TextComp.cpp
        src/Layer.cpp
        src/LayerStack.cpp
        src/Scene/ResourceManager.cpp
        src/Scene/Scene.cpp
        src/Scene/SceneManager.cpp
        src/Physics/CollisionDetector.cpp
        src/Physics/QuadTree.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            src/Application.h
            src/Core.h
            src/EntryPoint.h
            src/Layer.h
            src/LayerStack.h                     
            src/ECS/Component.h
            src/ECS/PhysComp.h
            src/ECS/SpriteComp.h
            src/ECS/TextComp.h
            src/ECS/TransComp.h
            src/ECS/Node/Entity.h
            src/ECS/Node/Node.h
            src/Scene/Scene.h
            src/Scene/SceneManager.h
            src/Scene/ResourceManager.h
            src/Spoon.h
            src/Physics/CollisionDetector.h
            src/Physics/PhysicsManager.h
            src/Physics/QuadTree.h
            src/Utils/MemoryUtils.h  
)
set(SFML_DIR "${CMAKE_CURRENT_LIST_DIR}/vendor/SFML-3.0.2/lib/cmake/SFML")

find_package(SFML REQUIRED System Audio Network Window Graphics)

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
    EXPORT special-spoon
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/special-spoon
    NAMESPACE special-spoon::
)