#pragma once

namespace Spoon
{
    class SystemManager;

    class SystemsMenu
    {
    public:
        SystemsMenu() {}
        ~SystemsMenu() {}

        void Open();
        bool IsOpen();
        void ReloadSystems();
        void Update(SystemManager& manager);
    private:
        bool isOpen = false;
        bool init = true;
    };
}