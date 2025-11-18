add_executable(Sandbox)

target_sources(Sandbox
    PRIVATE
        Sandbox.cpp
        DemoLayer.cpp
        DemoMenu.cpp
        DemoZombie.cpp
        MainMenuScene.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            DemoLayer.h
            DemoMenu.h
            DemoZombie.h
            MainMenuScene.h
)

include(GNUInstallDirs)

install(
    TARGETS Sandbox
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Sandbox
)