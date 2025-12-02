add_executable(Sandbox)

target_sources(Sandbox
    PRIVATE
        src/Sandbox.cpp
        src/DemoLayer.cpp
        src/DemoMenu.cpp
        src/DemoScene.cpp
        src/DemoZombie.cpp
        src/MainMenuScene.cpp
        src/Player.cpp
        src/SceneTransistion.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            src/DemoLayer.h
            src/DemoMenu.h
            src/DemoScene.h
            src/DemoZombie.h
            src/MainMenuScene.h
            src/Player.h
            src/SceneTransition.h
)

include(GNUInstallDirs)

install(
    TARGETS Sandbox
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/Sandbox
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Sandbox
)