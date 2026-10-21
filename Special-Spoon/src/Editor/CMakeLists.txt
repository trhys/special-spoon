target_sources(Special-Spoon
    PRIVATE
        Editor.h
        Editor.cpp
        Services.h

        Blueprints/Blueprint.h

        Tools/AnimationTool.h
        Tools/AnimationTool.cpp
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
)

add_subdirectory(Menus)