target_sources(Special-Spoon
    PRIVATE
        Application.cpp
        SceneManager.cpp    

        Application.h
        ComponentArray.h
        Core.h
        EntityManager.h
        EntryPoint.h
        Renderer.h
        SceneManager.h

        ResourceManager/ResourceManager.h
        ResourceManager/ResourceManager.cpp
        ResourceManager/ResourceManager.inl

        Serialization/ComponentLoaders.h
        Serialization/ComponentLoaders.cpp
        Serialization/SystemLoaders.h
        Serialization/SystemLoaders.cpp
        Serialization/Serializer.h
        Serialization/JsonTypeDefs.h
)