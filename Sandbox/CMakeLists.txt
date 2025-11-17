add_executable(Sandbox)

target_sources(Sandbox
    PRIVATE
        Sandbox.cpp
        DemoLayer.cpp
        DemoZombie.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            Background.h
            DemoLayer.h
            DemoZombie.h
)

include(GNUInstallDirs)

install(
    TARGETS Sandbox
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Sandbox
)