#include "ECS/Component/Combat/AttackComp.h"
#include "ECS/Component/Combat/Attack.h"

void AttackComp::DrawAttackEditor()
{
    static AttackDef attack{};
    static char id[128] = "";
    static char name[128] = "";
    if (ImGui::Begin("Attack Editor", &showAttackEditor))
    {
        if (ImGui::InputText("ID", id, IM_ARRAYSIZE(id)))
            attack.id = id;
        if (ImGui::InputText("Title", name, IM_ARRAYSIZE(name)))
            attack.name = name;
    }
    ImGui::End();
}

void AttackComp::OnReflect()
{
    ImGui::Text("Attack id: %s", selectedAttack.c_str());
    if (ImGui::Button("Open Editor"))
        showAttackEditor = true;
    if (showAttackEditor)
        DrawAttackEditor();
}