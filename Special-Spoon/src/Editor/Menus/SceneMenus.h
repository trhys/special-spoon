#pragma once

namespace Spoon
{
    class SceneManager;
    class EntityManager;
    class SystemManager;
    class Editor;

    void NewSceneMenu(SceneManager& s_Manager, Editor* editor);
    void LoadSceneMenu(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager, Editor* editor);
    void SaveSceneMenu(EntityManager& e_Manager, SystemManager& sys_Manager, Editor* editor);
    void SceneManifestMenu(SceneManager& s_Manager, Editor* editor);
}