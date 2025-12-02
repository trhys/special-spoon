#pragma once

#include "Spoon.h"

class DemoScene : public Spoon::Scene
{
public:
    DemoScene();
    ~DemoScene() {}

    void OnTransition() override;
    void OnCache() override;
    void OnStart() override;
    void OnEnd() override;

private:
    void OnUpdate(sf::Time tick) override;

    sf::Time timer;
    bool transitioning = false;
    bool is_Initialized = false;
};

class Arena : public Spoon::Entity
{
public:
    Arena(sf::Texture& asset) { AddComponent<SpriteComp>(asset); }
    ~Arena() {}

private:
    void OnUpdate(sf::Time tick) override
    {
        m_Timer = m_Timer + tick;
        if(m_Timer.asSeconds() > 10)
        {
            AddChild<DemoZombie>(GET_TEXTURE("demozombie"), {500.0f, 500.0f});
            AddChild<DemoZombie>(GET_TEXTURE("demozombie"), {200.0f, 800.0f});
            m_Timer.Zero;
        }
    }

private:
    sf::Time m_Timer;
};