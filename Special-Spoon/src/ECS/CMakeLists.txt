target_sources(Special-Spoon
    PRIVATE
        Components/Component.h
        
        Components/Core/InputComp.h
        Components/Core/InputComp.cpp
        Components/Core/StatusComp.h
        Components/Core/StatusComp.cpp
        Components/Core/TransformComp.h
        Components/Core/TransformComp.cpp

        Components/Animation/AnimationComp.h
        Components/Animation/AnimationData.h
        Components/Animation/BlinkComp.h
        Components/Animation/FadeComp.h

        Components/Physics/ColliderComp.h
        Components/Physics/PhysicsComp.h
        Components/Physics/MovementComp.h

        Components/Render/ParticleEmitterComp.h
        Components/Render/ParticleEmitterComp.cpp
        Components/Render/SpriteComp.h
        Components/Render/SpriteComp.cpp
        Components/Render/TextComp.h
        Components/Render/TextComp.cpp
        Components/Render/ColorComp.h
        Components/Render/RenderLayer.h
        Components/Render/UI/UIComp.h
        Components/Render/UI/UIComp.cpp
        
        Components/World/TileMapComp.h
        Components/World/TileMapComp.cpp

        Components/Sound/AudioComp.h
        Components/Sound/AudioComp.cpp
        
        ComponentReaper.h
        ComponentReaper.cpp
        ECS.h
        Entity.h
        Entity.inl
        UUID.h
)
