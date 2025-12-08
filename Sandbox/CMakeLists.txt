add_executable(Sandbox)

target_link_libraries(Sandbox PRIVATE special-spoon-core)

target_sources(Sandbox
    PRIVATE
        src/Sandbox.cpp


    PUBLIC
        FILE_SET HEADERS
        FILES

)

target_include_directories(Sandbox PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/special-spoon-core/src
${CMAKE_BINARY_DIR}/special-spoon-core)

include(GNUInstallDirs)

install(
    TARGETS Sandbox
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Sandbox
)