add_library(Special-Spoon SHARED)

# generate system assets
set(system_assets_dir
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ResourceManager/system"
)
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${generated_dir}")

foreach(asset IN ITEMS special-spoon.png folder.png)
    set(asset_path "${system_assets_dir}/${asset}")

    # Regenerate when the source image changes.
    set_property(DIRECTORY APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS "${asset_path}"
    )

    string(MAKE_C_IDENTIFIER "${asset}" symbol)
    file(READ "${asset_path}" hex HEX)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1,\n" bytes "${hex}")

    file(WRITE "${generated_dir}/${symbol}.h"
        "#pragma once\n"
        "namespace Spoon::SystemAssets {\n"
        "inline constexpr unsigned char ${symbol}[] = {\n"
        "${bytes}"
        "};\n"
        "}\n"
    )
endforeach()

target_include_directories(Special-Spoon PRIVATE "${generated_dir}")

# require explicit symbol exports regardless of platform
# otherwise unix will hide issues that will come up on windows
# see issue #92
set_target_properties(Special-Spoon PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

target_sources(Special-Spoon
    PRIVATE
        src/Spoon.h
)

add_subdirectory(src/Core)
add_subdirectory(src/ECS)
add_subdirectory(src/Editor)
add_subdirectory(src/System)
add_subdirectory(src/Utils)

generate_export_header(Special-Spoon)

target_include_directories(Special-Spoon PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# SFML
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build SFML as shared libraries" FORCE)
add_subdirectory(vendor/SFML)

target_link_libraries(Special-Spoon PUBLIC
    SFML::System
    SFML::Audio
    SFML::Network
    SFML::Window
    SFML::Graphics
)

# JSON
set(JSON_Install ON CACHE BOOL
    "Force nlohmann_json to be installable when used as a subdirectory."
    FORCE
)
add_subdirectory(vendor/JSON)

target_link_libraries(Special-Spoon PUBLIC
    nlohmann_json::nlohmann_json
)

find_package(OpenGL REQUIRED)

# building imgui into special-spoon as part of the library itself
# this avoids the need to separately build and link ImGui and its backends
# thereby running into global symbol conflicts. see #92
target_sources(Special-Spoon PRIVATE
    vendor/Imgui/imgui.cpp
    vendor/Imgui/imgui_demo.cpp
    vendor/Imgui/imgui_draw.cpp
    vendor/Imgui/imgui_tables.cpp
    vendor/Imgui/imgui_widgets.cpp
    vendor/Imgui/misc/cpp/imgui_stdlib.cpp

    vendor/Imgui-sfml/imgui-SFML.cpp

    vendor/ImGuiFileDialog/ImGuiFileDialog.cpp
)

target_include_directories(Special-Spoon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/Imgui>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/Imgui-sfml>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/ImGuiFileDialog
)

target_compile_definitions(Special-Spoon PUBLIC
    SPOON_IMGUI_EMBEDDED=1
    IMGUI_USER_CONFIG=\"imconfig-SFML.h\"
)

target_link_libraries(Special-Spoon PRIVATE OpenGL::GL)

if(WIN32 AND MINGW)
    target_link_libraries(Special-Spoon PRIVATE imm32)
endif()

# END LIBS SETUP

include(GNUInstallDirs)

install(
    TARGETS Special-Spoon
    EXPORT Special-Spoon
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/Special-Spoon
)

file(GLOB_RECURSE CORE_HDRS CONFIGURE_DEPENDS "src/*.h")

install(
    FILES ${CORE_HDRS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Special-Spoon
)

install(
    EXPORT Special-Spoon
    FILE special-spoon-targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Special-Spoon
    NAMESPACE Special-Spoon::
)

install(
    FILES
        ${CMAKE_SOURCE_DIR}/cmake/special-spoonConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Special-Spoon
)