add_library(special-spoon-core SHARED)

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

generate_export_header(special-spoon-core)

target_include_directories(special-spoon-core PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_BINARY_DIR})

# set(SFML_DIR "${CMAKE_CURRENT_LIST_DIR}/vendor/SFML-3.0.2/lib/cmake/SFML")

# find_package(SFML REQUIRED System Audio Network Window Graphics)

add_subdirectory(vendor/SFML)

target_link_libraries(special-spoon-core PUBLIC 
SFML::System
SFML::Audio
SFML::Network
SFML::Window
SFML::Graphics)

include(GNUInstallDirs)

install(
    TARGETS special-spoon-core
    EXPORT special-spoon
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/special-spoon
)

file(GLOB_RECURSE CORE_HDRS CONFIGURE_DEPENDS "src/*.h")

install(
    FILES ${CORE_HDRS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/special-spoon-core
)

install(
    EXPORT special-spoon
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/special-spoon
    NAMESPACE special-spoon::
)