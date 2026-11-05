target_sources(Special-Spoon
    PRIVATE
        Editor.h
        Editor.cpp
        Services.h

        Blueprints/Blueprint.h
        Blueprints/DefaultBlueprints.h
        Blueprints/DefaultBlueprints.cpp
        Blueprints/ProjectBlueprint.h
        Blueprints/ProjectBlueprint.cpp

        Tools/Animation/AnimationTool.h
        Tools/Animation/AnimationTool.cpp
        Tools/Animation/File.cpp
        Tools/TileMapTool.h
        Tools/TileMapTool.cpp
        Tools/TextureRectTool.h

        Utils/Helpmarker.h    
        Utils/Tooltips.h
        Utils/Tooltips.cpp
        Utils/Viewport.h
        Utils/Viewport.cpp
        Utils/Windows.h
        Utils/Windows.cpp

        Widgets/Inspector.cpp
        Widgets/MenuBar.cpp
        Widgets/Metrics.cpp
        Widgets/LogConsole.cpp
)

add_subdirectory(Menus)