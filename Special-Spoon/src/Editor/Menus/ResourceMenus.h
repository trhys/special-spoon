#pragma once

namespace Spoon
{
    struct AssetNode;
    class Editor;

    void ViewResourcesMenu(Editor* editor);
    void LoadResourcesMenu(Editor* editor);
    void ViewAssets(AssetNode* node);
    void ShowTree(AssetNode* node);
}