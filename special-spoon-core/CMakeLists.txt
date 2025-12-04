add_library(special-spoon-core SHARED)

target_sources(special-spoon-core
    PRIVATE
        src/Application.cpp
        src/Layer.cpp
        src/LayerStack.cpp
        src/Scene/ResourceManager.cpp
        src/Scene/Scene.cpp
        src/Scene/SceneManager.cpp
        src/Physics/CollisionDetector.cpp
        src/Physics/QuadTree.cpp
        src/System/AnimationSystem.cpp
        
        # HEADERS
        
        src/Application.h
        src/Core.h
        src/EntryPoint.h
        src/Layer.h
        src/LayerStack.h  
        src/Spoon.h

        src/ECS/Components/Component.h
        src/ECS/Components/InputComp.h
        src/ECS/Components/PhysicsComp.h
        src/ECS/Components/StatusComp.h
        src/ECS/Components/TransformComp.h
        src/ECS/Components/Animation/BlinkComp.h
        src/ECS/Components/Animation/FadeComp.h
        src/ECS/Components/Render/SpriteComp.h
        src/ECS/Components/Render/TextComp.h
        src/ECS/Components/Render/ColorComp.h
        
        src/ECS/Entity.h
        src/ECS/ECS.h

        src/Scene/Scene.h
        src/Scene/SceneManager.h
        src/Scene/ResourceManager.h

        src/System/AnimationSystem.h
        src/System/ComponentArray.h
        src/System/EntityManager.h
        src/System/InputSystem.h
        src/System/RenderSystem.h

        src/Physics/CollisionDetector.h
        src/Physics/PhysicsManager.h
        src/Physics/QuadTree.h
        src/Utils/MemoryUtils.h  
        src/Utils/KeyMappings.h
)

generate_export_header(special-spoon-core)

target_include_directories(special-spoon-core PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(vendor)

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