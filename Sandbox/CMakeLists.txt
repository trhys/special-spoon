add_executable(Sandbox)

target_sources(Sandbox
    PRIVATE
        Sandbox.cpp
        DemoLayer.cpp
        DemoMenu.cpp
        DemoScene.cpp
        DemoZombie.cpp
        MainMenuScene.cpp
        Player.cpp
        SceneTransition.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            DemoLayer.h
            DemoMenu.h
            DemoScene.h
            DemoZombie.h
            MainMenuScene.h
            Player.h
            SceneTransition.h
)

include(GNUInstallDirs)

install(
    TARGETS Sandbox
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Sandbox
)