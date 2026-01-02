#pragma once

namespace Spoon
{
    class Editor;
    class SystemManager;

    void ReloadSystems();
    void SystemsMenu(SystemManager& manager, Editor* editor);
}