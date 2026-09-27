target_sources(Special-Spoon
    PRIVATE
        Editor.h
        Editor.cpp
        Overlay.h
        Overlay.cpp

        Blueprints/Blueprint.h

        Tools/AnimationTool.h
        Tools/AnimationTool.cpp
        Tools/TileMapTool.h
        Tools/TileMapTool.cpp
        Tools/TextureRectTool.h

        Utils/Helpmarker.h        
        Utils/Viewport.h
        Utils/Viewport.cpp
)

add_subdirectory(Menus)