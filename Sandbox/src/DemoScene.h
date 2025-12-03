#pragma once

#include "Spoon.h"
#include "DemoZombie.h"

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
    Arena(sf::Texture& asset) { AddComponent<Spoon::SpriteComp>(asset); }
    ~Arena() {}

private:
    void OnUpdate(sf::Time tick) override
    {
        m_Timer = m_Timer + tick;
        if(m_Timer.asSeconds() > 10)
        {
            AddChild<DemoZombie>(Spoon::ResourceManager::GET_TEXTURE("demozombie"), sf::Vector2f{500.0f, 500.0f});
            AddChild<DemoZombie>(Spoon::ResourceManager::GET_TEXTURE("demozombie"), sf::Vector2f{200.0f, 800.0f});
            m_Timer.Zero;
        }
    }

private:
    sf::Time m_Timer;
};