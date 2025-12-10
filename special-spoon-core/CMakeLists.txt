add_library(special-spoon-core SHARED)

target_sources(special-spoon-core
    PRIVATE
        src/Core/Application.cpp
        src/Core/ComponentLoaders.cpp
        src/Core/SceneManager.cpp

        src/System/Physics/QuadTree.cpp

        src/System/AnimationSystem.cpp
        src/System/SystemLoaders.cpp
        
        src/ECS/Entity.inl

        # ===============
        # === HEADERS ===
        # ===============

        src/Spoon.h

        src/Core/Application.h
        src/Core/ComponentArray.h
        src/Core/ComponentLoaders.h
        src/Core/Core.h
        src/Core/EntityManager.h
        src/Core/EntryPoint.h
        src/Core/Renderer.h
        src/Core/ResourceManager.h
        src/Core/SceneManager.h

        src/ECS/Components/Component.h
        src/ECS/Components/InputComp.h
        src/ECS/Components/PhysicsComp.h
        src/ECS/Components/StatusComp.h
        src/ECS/Components/TransformComp.h

        src/ECS/Components/Animation/AnimationComp.h
        src/ECS/Components/Animation/AnimationData.h
        src/ECS/Components/Animation/BlinkComp.h
        src/ECS/Components/Animation/FadeComp.h

        src/ECS/Components/Render/SpriteComp.h
        src/ECS/Components/Render/TextComp.h
        src/ECS/Components/Render/ColorComp.h
        
        src/ECS/Entity.h
        src/ECS/ECS.h

        src/System/AnimationSystem.h
        src/System/InputSystem.h
        src/System/System.h
        src/System/SystemLoaders.h
        src/System/SystemManager.h

        src/System/Physics/PhysicsSystem.h
        src/System/Physics/QuadTree.h

        src/Utils/MemoryUtils.h  
        src/Utils/KeyMappings.h
)

   
generate_export_header(special-spoon-core)

target_include_directories(special-spoon-core PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(vendor/SFML)

target_link_libraries(special-spoon-core PUBLIC 
SFML::System
SFML::Audio
SFML::Network
SFML::Window
SFML::Graphics)

set(JSON_Install ON CACHE BOOL "Force nlohmann_json to be installable when used as a subdirectory." FORCE)

add_subdirectory(vendor/JSON)

target_link_libraries(special-spoon-core PUBLIC nlohmann_json::nlohmann_json)

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
    FILE special-spoon-targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/special-spoon
    NAMESPACE special-spoon::
)

install(
    FILES
        ${CMAKE_SOURCE_DIR}/cmake/special-spoonConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/special-spoon
)