target_sources(Special-Spoon
    PRIVATE
        ActionQueue.h
        Application.h
        Application.cpp
        ComponentArray.h
        Core.h
        EntryPoint.h
        SceneManager.h
        SceneManager.cpp

        EntityManager/EntityManager.h
        EntityManager/EntityManager.inl
        EntityManager/EntityManager.cpp

        Registers/ActionRegistry.h
        Registers/StateRegistry.h
        Registers/Component/ComponentRegistry.h
        Registers/Component/ComponentRegistry.cpp
        Registers/System/SystemRegistry.h
        Registers/System/SystemRegistry.cpp

        Renderer/Renderer.h
        Renderer/Renderer.cpp
        Renderer/Renderable.h
        Renderer/WorldProjection.h
        
        ResourceManager/ResourceManager.h
        ResourceManager/ResourceManager.cpp
        ResourceManager/ResourceManager.inl

        Serialization/Serializer.h
        Serialization/Serializer.cpp
        Serialization/JsonTypeDefs.h
)

add_subdirectory(Project)