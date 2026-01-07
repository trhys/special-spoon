target_sources(Special-Spoon
    PRIVATE
        Components/Component.h
        Components/InputComp.h
        Components/InputComp.cpp
        Components/PhysicsComp.h
        Components/StateActionComp.h
        Components/StatusComp.h
        Components/TransformComp.h
        Components/TransformComp.cpp

        Components/Animation/AnimationComp.h
        Components/Animation/AnimationData.h
        Components/Animation/BlinkComp.h
        Components/Animation/FadeComp.h

        Components/Render/SpriteComp.h
        Components/Render/SpriteComp.cpp
        Components/Render/TextComp.h
        Components/Render/ColorComp.h
        Components/Render/RenderLayer.h
        
        ECS.h
        Entity.h
        Entity.inl
        UUID.h
)