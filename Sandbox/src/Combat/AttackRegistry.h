#pragma once

#include "ECS/Component/Combat/Attack.h"

class AttackRegistry
{
public:
    static AttackRegistry& Get()
    {
        static AttackRegistry instance;
        return instance;
    }

    void RegisterAttack(const AttackDef& attack)
    {
        if (GetAttack(attack.id))
            return;
        m_Attacks.push_back(attack);
    }

    const std::vector<AttackDef>& GetAttacks() const
    {
        return m_Attacks;
    }

    const AttackDef* GetAttack(const std::string& id) const
    {
        for (const auto& attack : m_Attacks)
        {
            if (attack.id == id)
                return &attack;
        }
        return nullptr;
    }

    void ClearRegister()
    {
        m_Attacks.clear();
    }

private:
    AttackRegistry() = default;
    std::vector<AttackDef> m_Attacks;
};