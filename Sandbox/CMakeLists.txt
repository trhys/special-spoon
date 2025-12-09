add_executable(Sandbox)

if(DEBUG_MODE)
    target_compile_definitions(Sandbox PRIVATE SS_DEBUG_ENABLED SS_ASSERTS_ENABLED)
endif()

if(CONSOLE_ONLY)
    target_compile_definitions(Sandbox PRIVATE SS_CONSOLE_ONLY)
endif()

target_link_libraries(Sandbox PRIVATE special-spoon-core)

target_sources(Sandbox
    PRIVATE
        src/Sandbox.cpp


    PUBLIC
        FILE_SET HEADERS
        FILES
            src/ECS/Component/PatrolComp.h
            src/ECS/System/PatrolSystem.h
            
            src/ECS/CustomCompLoader.h
            src/ECS/CustomSystemLoader.h
)

target_include_directories(Sandbox PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/special-spoon-core/src
${CMAKE_BINARY_DIR}/special-spoon-core)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_DESTINATION_DIR "$<TARGET_FILE_DIR:Sandbox>/assets")

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ASSETS_SOURCE_DIR}"
        "${ASSETS_DESTINATION_DIR}"
    COMMENT "Copying assets to ${ASSETS_DESTINATION_DIR}")

add_dependencies(Sandbox copy_assets)

include(GNUInstallDirs)

install(
    TARGETS Sandbox
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Sandbox
)