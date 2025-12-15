add_library(special-spoon-core SHARED)

target_sources(special-spoon-core
    PRIVATE
        src/Spoon.h
        src/Editor/Editor.h
        src/Editor/Editor.cpp
)

add_subdirectory(src/Core)
add_subdirectory(src/ECS)
add_subdirectory(src/System)
add_subdirectory(src/Utils)

generate_export_header(special-spoon-core)

target_include_directories(special-spoon-core PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# IMGUI SETUP =====================

add_library(ImGui STATIC 
    vendor/Imgui/imgui.cpp
    vendor/Imgui/imgui_demo.cpp
    vendor/Imgui/imgui_draw.cpp
    vendor/Imgui/imgui_tables.cpp
    vendor/Imgui/imgui_widgets.cpp
)

target_include_directories(ImGui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Imgui
)

# SFML SETUP =====================

add_subdirectory(vendor/SFML)

target_link_libraries(special-spoon-core PUBLIC 
SFML::System
SFML::Audio
SFML::Network
SFML::Window
SFML::Graphics)

# JSON SETUP =====================

set(JSON_Install ON CACHE BOOL "Force nlohmann_json to be installable when used as a subdirectory." FORCE)

add_subdirectory(vendor/JSON)

target_link_libraries(special-spoon-core PUBLIC nlohmann_json::nlohmann_json)

# IMGUI-SFML BACKEND =====================

set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "Use find_package to find SFML" FORCE)
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Imgui" CACHE PATH "Imgui top-level directory")
add_subdirectory(vendor/Imgui-sfml)

target_link_libraries(special-spoon-core PUBLIC ImGui-SFML)

# END LIBS SETUP =====================

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