#pragma once

namespace Spoon
{
    struct AssetNode;
    
    // center the next appearing window
    void CenterNext();

    // non editor-owned components
    void DrawTextureBrowser(AssetNode*& selectedNode);
}