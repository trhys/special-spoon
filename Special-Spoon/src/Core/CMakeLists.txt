target_sources(Special-Spoon
    PRIVATE
        ActionQueue.h
        Application.h
        Application.cpp
        ComponentArray.h
        Core.h
        EntryPoint.h
        ProjectManager.h
        ProjectManager.cpp
        Renderer.h
        SceneManager.h
        SceneManager.cpp

        EntityManager/EntityManager.h
        EntityManager/EntityManager.inl
        EntityManager/EntityManager.cpp

        Registers/ActionRegistry.h
        Registers/Component/ComponentRegistry.h
        Registers/Component/ComponentRegistry.cpp
        Registers/System/SystemRegistry.h
        Registers/System/SystemRegistry.cpp
        
        ResourceManager/ResourceManager.h
        ResourceManager/ResourceManager.cpp
        ResourceManager/ResourceManager.inl
        ResourceManager/SystemFont/Jersey_10/SystemFont.h

        Serialization/Serializer.h
        Serialization/Serializer.cpp
        Serialization/JsonTypeDefs.h
)