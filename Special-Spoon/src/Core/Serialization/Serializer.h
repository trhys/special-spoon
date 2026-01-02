#pragma once

namespace Spoon
{
    class EntityManager;
    class SystemManager;
    class SceneManager;
    struct SceneData;

    // Serialize scene data from EntityManager and SystemManager into scene data files
    void Serialize(SceneData scene, EntityManager& e_Manager, SystemManager& s_Manager);

    // Serialize the scene manifest from the SceneManager to disk
    void SerializeManifest(SceneManager& s_Manager);
}