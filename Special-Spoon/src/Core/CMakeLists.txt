target_sources(Special-Spoon
    PRIVATE
        Application.cpp
        SceneManager.cpp    

        Application.h
        ComponentArray.h
        Core.h
        EntryPoint.h
        Renderer.h
        SceneManager.h

        EntityManager/EntityManager.h
        EntityManager/EntityManager.inl
        EntityManager/EntityManager.cpp

        ResourceManager/ResourceManager.h
        ResourceManager/ResourceManager.cpp
        ResourceManager/ResourceManager.inl
        ResourceManager/SystemFont/Jersey_10/SystemFont.h

        Serialization/ComponentLoaders.h
        Serialization/ComponentLoaders.cpp
        Serialization/SystemLoaders.h
        Serialization/SystemLoaders.cpp
        Serialization/Serializer.h
        Serialization/Serializer.cpp
        Serialization/JsonTypeDefs.h
)