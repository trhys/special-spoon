#pragma once

#include <functional>

namespace Spoon
{
    using EditorAction = std::function<void()>;

    struct EditorActionGroup {
        std::vector<EditorAction> actions;

        void addAction(EditorAction action) {
            actions.push_back(std::move(action));
        }
        void executeAll() {
            for (const auto& action : actions) {
                action();
            }
        }
    };
}