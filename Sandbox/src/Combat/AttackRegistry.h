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
        m_Attacks.push_back(attack);
    }

    const std::vector<AttackDef>& GetAttacks() const
    {
        return m_Attacks;
    }

    const AttackDef* GetAttack(const std::string& name) const
    {
        for (const auto& attack : m_Attacks)
        {
            if (attack.name == name)
                return &attack;
        }
        return nullptr;
    }

private:
    AttackRegistry() = default;
    std::vector<AttackDef> m_Attacks;
};