add_executable(Sandbox)

target_link_libraries(Sandbox PRIVATE Special-Spoon)

target_sources(Sandbox
    PUBLIC
        src/Sandbox.h
)

add_subdirectory(src/client)

target_include_directories(Sandbox PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/Special-Spoon/src
${CMAKE_SOURCE_DIR}/Special-Spoon/vendor
${CMAKE_BINARY_DIR}/Special-Spoon)

# Copy all runtime DLL dependencies (SFML, Special-Spoon, etc.) next to Sandbox.exe
if(WIN32)
    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:Sandbox>
            $<TARGET_FILE_DIR:Sandbox>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLL dependencies to $<TARGET_FILE_DIR:Sandbox>"
    )
endif()

include(GNUInstallDirs)

install(
    TARGETS Sandbox
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Sandbox
)
