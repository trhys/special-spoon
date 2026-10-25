add_executable(Sandbox)

target_link_libraries(Sandbox PRIVATE Special-Spoon)

target_sources(Sandbox
    PRIVATE
        src/Sandbox.cpp
        src/Combat/AttackSerialization.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            src/Combat/AttackRegistry.h
            src/Combat/AttackSerialization.h
)

add_subdirectory(src/ECS)

target_include_directories(Sandbox PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/Special-Spoon/src
${CMAKE_SOURCE_DIR}/Special-Spoon/vendor
${CMAKE_BINARY_DIR}/Special-Spoon)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_DESTINATION_DIR "$<TARGET_FILE_DIR:Sandbox>/assets")
set(DATA_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(DATA_DESTINATION_DIR "$<TARGET_FILE_DIR:Sandbox>/data")

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ASSETS_SOURCE_DIR}"
        "${ASSETS_DESTINATION_DIR}"
    COMMENT "Copying assets to ${ASSETS_DESTINATION_DIR}")

add_custom_target(copy_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${DATA_SOURCE_DIR}"
        "${DATA_DESTINATION_DIR}"
    COMMENT "Copying data files to ${DATA_DESTINATION_DIR}")

add_dependencies(Sandbox copy_assets)

# Copy all runtime DLL dependencies (SFML, Special-Spoon, etc.) next to Sandbox.exe
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:Sandbox>
        $<TARGET_FILE_DIR:Sandbox>
    COMMAND_EXPAND_LISTS
    COMMENT "Copying runtime DLL dependencies to $<TARGET_FILE_DIR:Sandbox>"
)

include(GNUInstallDirs)

install(
    TARGETS Sandbox
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Sandbox
)
