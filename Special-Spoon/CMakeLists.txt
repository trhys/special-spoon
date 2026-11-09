add_library(Special-Spoon SHARED)

# pch
target_precompile_headers(Special-Spoon PRIVATE
    <algorithm>
    <memory>
    <string>
    <unordered_map>
    <vector>
    <cstdint>
    <SFML/Graphics.hpp>
    <nlohmann/json.hpp>
    <imgui.h>
)

# generate system assets
set(system_assets_dir
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ResourceManager/system"
)
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${generated_dir}")

foreach(asset IN ITEMS special-spoon.png folder.png Jersey10-Regular.ttf)
    set(asset_path "${system_assets_dir}/${asset}")

    # Regenerate when the source asset changes.
    set_property(DIRECTORY APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS "${asset_path}"
    )

    string(MAKE_C_IDENTIFIER "${asset}" symbol)
    file(READ "${asset_path}" hex HEX)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1,\n" bytes "${hex}")

    string(CONCAT header_content
        "#pragma once\n"
        "namespace Spoon::SystemAssets {\n"
        "inline constexpr unsigned char ${symbol}[] = {\n"
        "${bytes}"
        "};\n"
        "}\n"
    )
    file(GENERATE 
        OUTPUT "${generated_dir}/${symbol}.h"
        CONTENT "${header_content}"
    )
endforeach()

target_include_directories(Special-Spoon PRIVATE "${generated_dir}")

# embed the default blueprint JSON files so built-in blueprints are
# available in development and installed builds without working-directory
# or source-tree dependencies
set(blueprint_defaults_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/ECS/Blueprints/Defaults")
file(GLOB blueprint_defaults CONFIGURE_DEPENDS "${blueprint_defaults_dir}/*.json")
list(SORT blueprint_defaults)

set(blueprint_entries "")
foreach(blueprint IN LISTS blueprint_defaults)
    # Regenerate when a blueprint changes.
    set_property(DIRECTORY APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS "${blueprint}"
    )

    get_filename_component(blueprint_name "${blueprint}" NAME_WE)
    file(READ "${blueprint}" blueprint_json)
    string(APPEND blueprint_entries
        "    { \"${blueprint_name}\", R\"SpoonBP(${blueprint_json})SpoonBP\" },\n")
endforeach()

string(CONCAT blueprint_header_content
    "#pragma once\n"
    "namespace Spoon::EmbeddedBlueprints {\n"
    "struct Entry { const char* name; const char* data; };\n"
    "inline constexpr Entry entries[] = {\n"
    "${blueprint_entries}"
    "};\n"
    "}\n"
) 
file(GENERATE 
    OUTPUT "${generated_dir}/default_blueprints.h"
    CONTENT
        "${blueprint_header_content}"
)

# embed the built-in project templates (project blueprints plus the scene
# templates they reference), keyed by path relative to the template root
set(project_templates_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Project/ProjectTemplates")
file(GLOB_RECURSE project_templates CONFIGURE_DEPENDS "${project_templates_dir}/*.json")
list(SORT project_templates)

set(project_template_entries "")
foreach(template IN LISTS project_templates)
    # Regenerate when a template changes.
    set_property(DIRECTORY APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS "${template}"
    )

    file(RELATIVE_PATH template_key "${project_templates_dir}" "${template}")
    file(READ "${template}" template_json)
    string(APPEND project_template_entries
        "    { \"${template_key}\", R\"SpoonPT(${template_json})SpoonPT\" },\n")
endforeach()

string(CONCAT project_template_header_content
    "#pragma once\n"
    "namespace Spoon::EmbeddedProjectTemplates {\n"
    "struct Entry { const char* path; const char* data; };\n"
    "inline constexpr Entry entries[] = {\n"
    "${project_template_entries}"
    "};\n"
    "}\n"
)   
file(GENERATE 
    OUTPUT "${generated_dir}/project_templates.h"
    CONTENT
        "${project_template_header_content}"
)

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

# Lua 5.4: static runtime embedded in the engine
set(lua_dir "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua")

file(GLOB lua_sources CONFIGURE_DEPENDS "${lua_dir}/*.c")
list(REMOVE_ITEM lua_sources
    "${lua_dir}/lua.c"
    "${lua_dir}/luac.c"
    "${lua_dir}/ltests.c"
    "${lua_dir}/onelua.c"
)

add_library(spoon_lua STATIC ${lua_sources})

set_target_properties(spoon_lua PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden
)

target_include_directories(spoon_lua PUBLIC "${lua_dir}")

if(UNIX)
    target_compile_definitions(spoon_lua PRIVATE
        LUA_USE_POSIX
        LUA_USE_DLOPEN
    )
endif()

if(UNIX)
    target_link_libraries(spoon_lua PRIVATE m ${CMAKE_DL_LIBS})
endif()

# sol2 is header-only; use the vendored headers directly.
target_include_directories(Special-Spoon SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/sol2/include"
)

target_compile_definitions(Special-Spoon PRIVATE
    SOL_ALL_SAFETIES_ON=1
)

target_link_libraries(Special-Spoon PRIVATE spoon_lua)

# make sources skip pchs for vendor files
get_target_property(spoon_sources Special-Spoon SOURCES)

foreach(source IN LISTS spoon_sources)
    if(source MATCHES "^vendor/")
        set_source_files_properties("${source}" PROPERTIES
            SKIP_PRECOMPILE_HEADERS ON
        )
    endif()
endforeach()

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