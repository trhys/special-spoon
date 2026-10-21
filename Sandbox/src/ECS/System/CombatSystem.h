#pragma once

#include "Spoon.h"
#include "ECS/Component/Combat/AttackRegistery.h"

class CombatSystem : public Spoon::ISystem
{
public:
    CombatSystem() : Spoon::ISystem::ISystem("Combat") {}

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        UpdateActiveHitboxes(tick);

        // fetch action queue and arrays
        auto& actions = Spoon::Application::Get().GetActionQueue();
        auto& attackArray = manager.GetArray<AttackComp>(AttackComp::Name);
        auto& statusArray = manager.GetArray<Spoon::StatusComp>(Spoon::StatusComp::Name);

        // get attack action for filtering
        const auto attack = Spoon::ActionRegistry::Get().GetAction("Attack");

        // iterate attacking entities
        for (size_t index = 0; index < attackArray.m_Components.size(); index++)
        {
            AttackComp& attackComp = attackArray.m_Components[index];
            Spoon::UUID id = attackArray.m_IndexToId[index];

            // if already attacking -> move phase timer
            if (attackComp.isAttacking) {
                attackComp.phaseTimer += tick.asSeconds();
            }

            // always decrement cooldown if above 0
            if (attackComp.cooldownTimer > 0.0f) {
                attackComp.cooldownTimer -= tick.asSeconds();
                if (attackComp.cooldownTimer < 0.0f)
                    attackComp.cooldownTimer = 0.0f;
            }

            // check for attack action and set the attack state
            if (actions.HasAction(attack, id) 
                && !attackComp.isAttacking
                && !attackComp.cooldownTimer > 0.0f
            ) 
            {
                attackComp.isAttacking = true;
                attackComp.currentPhase = AttackPhase::WindUp;
                attackComp.phaseTimer = 0.0f;

                const AttackDef* attackDef = AttackRegistry::Get().GetAttack(attackComp.selectedAttack);
                if (attackDef) {
                    attackComp.cooldownTimer = attackDef->cooldownSec;
                }                
            }

            // apply phase changes
            if (attackComp.isAttacking) {
                const AttackDef* attackDef = AttackRegistry::Get().GetAttack(attackComp.selectedAttack);
                if (attackDef) {
                    switch (attackComp.currentPhase) {
                        case AttackPhase::WindUp:
                            if (attackComp.phaseTimer >= attackDef->windupSec) {
                                attackComp.currentPhase = AttackPhase::Strike;
                                attackComp.phaseTimer = 0.0f;
                                SpawnHitbox(attackDef, id, manager);
                            }
                            break;
                        case AttackPhase::Strike:
                            if (attackComp.phaseTimer >= attackDef->activeSec) {
                                attackComp.currentPhase = AttackPhase::Recovery;
                                attackComp.phaseTimer = 0.0f;
                            }
                            break;
                        case AttackPhase::Recovery:
                            if (attackComp.phaseTimer >= attackDef->recoverySec) {
                                attackComp.currentPhase = AttackPhase::None;
                                attackComp.isAttacking = false;
                                attackComp.phaseTimer = 0.0f;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }

            // apply attack state if attacking
            if (statusArray.m_IdToIndex.count(id) && attackComp.isAttacking)
            {
                auto& statusComp = statusArray.m_Components[statusArray.m_IdToIndex[id]];
                statusComp.m_CurrentState = Spoon::StateRegistry::Get().GetState("Attack");
            }
        }

        
        // resolve hitboxes vs hurtboxes and apply damage
        FindHits(manager);
        ResolveHits(manager);
    }

    void UpdateActiveHitboxes(const sf::Time& tick)
    {
        const float dt = tick.asSeconds();

        for (auto it = m_ActiveHitboxes.begin();
            it != m_ActiveHitboxes.end();)
        {
            auto& hitbox = *it;
            hitbox->lifetimeSec -= dt;

            if (hitbox->lifetimeSec <= 0.0f)
                it = m_ActiveHitboxes.erase(it);
            else
                ++it;
        }
    }

    void SpawnHitbox(const AttackDef* attack, const Spoon::UUID owner, Spoon::EntityManager& manager)
    {
        if (!attack)
            return;

        std::unique_ptr<Hitbox> hitbox = std::make_unique<Hitbox>(*attack);
        hitbox->owner = owner;
        hitbox->attackInstanceId = m_NextAttackInstanceId++;
        
        // build floatrect at owner transform
        auto& transform = manager.GetComponent<Spoon::TransformComp>(owner, Spoon::TransformComp::Name);
        hitbox->box = BuildHitboxBounds(attack, transform);

        m_ActiveHitboxes.push_back(std::move(hitbox));
    }

    sf::FloatRect BuildHitboxBounds(const AttackDef* attack, Spoon::TransformComp& transform)
    {
        sf::FloatRect bounds{};
        if (!attack)
            return bounds;

        const sf::Vector2f position = transform.GetPosition();
        const char* facing = transform.GetFacingDirection();

        // default dimensions (horizontal)
        const float width  = attack->halfExtents.x * 2.f;
        const float height = attack->halfExtents.y * 2.f;

        // for vertical facings, swap dims so thrust/slash orientation makes sense
        const bool vertical = std::strcmp(facing, "Up") == 0 || std::strcmp(facing, "Down") == 0;
        const float boxW = vertical ? height : width;
        const float boxH = vertical ? width  : height;

        // center offset in local "forward/side" terms projected into world axes
        sf::Vector2f centerOffset{0.f, 0.f};

        if (std::strcmp(facing, "Right") == 0)      centerOffset = { attack->offset.x,  attack->offset.y };
        else if (std::strcmp(facing, "Left") == 0)  centerOffset = { -attack->offset.x, -attack->offset.y };
        else if (std::strcmp(facing, "Up") == 0)    centerOffset = { -attack->offset.y, -attack->offset.x };
        else if (std::strcmp(facing, "Down") == 0)  centerOffset = {  attack->offset.y,  attack->offset.x };
        else if (std::strcmp(facing, "UpRight") == 0)
        {
            const float d = attack->offset.x * 0.70710678f;
            centerOffset = { d, -d };
        }
        else if (std::strcmp(facing, "UpLeft") == 0)
        {
            const float d = attack->offset.x * 0.70710678f;
            centerOffset = { -d, -d };
        }
        else if (std::strcmp(facing, "DownRight") == 0)
        {
            const float d = attack->offset.x * 0.70710678f;
            centerOffset = { d, d };
        }
        else if (std::strcmp(facing, "DownLeft") == 0)
        {
            const float d = attack->offset.x * 0.70710678f;
            centerOffset = { -d, d };
        }
        else
        {
            centerOffset = { attack->offset.x, attack->offset.y };
        }

        const sf::Vector2f center = position + centerOffset;
        bounds.position = { center.x - boxW * 0.5f, center.y - boxH * 0.5f };
        bounds.size     = { boxW, boxH };
        return bounds;
    }

    void FindHits(Spoon::EntityManager& manager)
    {
        auto& hurtboxArray = manager.GetArray<HurtboxComp>(HurtboxComp::Name);
        auto& transformArray = manager.GetArray<Spoon::TransformComp>(Spoon::TransformComp::Name);

        for (size_t i = 0; i < hurtboxArray.m_Components.size(); i++)
        {
            auto& hurtbox = hurtboxArray.m_Components[i];
            auto id = hurtboxArray.m_IndexToId[i];

            // sync hurtbox to entity transform
            auto& transComp = transformArray.m_Components[transformArray.m_IdToIndex[id]];
            hurtbox.box.position = transComp.GetPosition();

            if (!hurtbox.enabled || hurtbox.isInvincible)
                continue;

            // check overlap to active hitboxes
            for (auto& hitbox : m_ActiveHitboxes)
            {
                if (hitbox->owner == id && !hitbox->canHitSelf)
                    continue;
                if (hitbox->box.findIntersection(hurtbox.box) && !hitbox->timesToHit.count(id)) {
                    hitbox->AddHit(id);
                }
            }
        }
    }

    void ResolveHits(Spoon::EntityManager& manager)
    {
        if (m_ActiveHitboxes.empty())
            return;

        auto& healthArray = manager.GetArray<HealthComp>(HealthComp::Name);
        auto& hurtboxArray = manager.GetArray<HurtboxComp>(HurtboxComp::Name);

        for (auto& hitbox : m_ActiveHitboxes)
        {
            for (Spoon::UUID hitId : hitbox->hitEntities)
            {
                if (!healthArray.m_IdToIndex.count(hitId))
                    continue;

                if (!hurtboxArray.m_IdToIndex.count(hitId))
                    continue;

                int& hits = hitbox->timesToHit[hitId];
                if (hits > 0)
                {
                    auto& hurtbox = hurtboxArray.m_Components[hurtboxArray.m_IdToIndex[hitId]];
                    if (hurtbox.isInvincible)
                        continue;
                    
                    auto& health = healthArray.m_Components[healthArray.m_IdToIndex[hitId]];
                    health.m_CurrentHealth -= hitbox->damage;
                    hits--;
                }
            } 
        }
    }

private:
    std::vector<std::unique_ptr<Hitbox>> m_ActiveHitboxes;
    std::uint64_t m_NextAttackInstanceId = 1;
};