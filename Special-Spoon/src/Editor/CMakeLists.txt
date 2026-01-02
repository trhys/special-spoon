target_sources(Special-Spoon
    PRIVATE
        Editor.h
        Editor.cpp

        Blueprints/Blueprint.h

        Tools/AnimationTool.h
        Tools/AnimationTool.cpp
        Tools/TextureRectTool.h

        Utils/Helpmarker.h        
        Utils/Viewport.h
        Utils/Viewport.cpp
)

add_subdirectory(Menus)