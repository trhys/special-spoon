#pragma once

#include "Spoon.h"
#include "DemoZombie.h"

class MainMenu : public Spoon::Scene
{
public:
    MainMenu();
    ~MainMenu() {}

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

class MenuScreen : public Spoon::Entity
{
public:
    MenuScreen(sf::Texture& asset) { AddComponent<Spoon::SpriteComp>(asset); }
    ~MenuScreen() {}

private:
    void OnUpdate(sf::Time tick) override
    {
        m_Timer = m_Timer + tick;
        if(m_Timer.asSeconds() > 10)
        {
            AddChild<DemoZombie>(Spoon::ResourceManager::GET_TEXTURE("demozombie"), sf::Vector2f{-100.0f, 600.0f});
            m_Timer.Zero;
        }
    }

private:
    sf::Time m_Timer;
};

class MenuText : public Spoon::Entity
{
public:
    MenuText(sf::Font& asset) { AddComponent<Spoon::TextComp>(asset, "Press Enter to Start", sf::Vector2f{400.0f, 400.0f}, true); }
    ~MenuText() {}

private:
    void OnUpdate(sf::Time tick) override
    {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab))
        { 
            if(!m_Switch) { GetComponent<Spoon::TextComp>()->SetColor(sf::Color::Red); }
            else { GetComponent<Spoon::TextComp>()->SetColor(sf::Color::White); }
            m_Switch = !m_Switch;
        }
    }

private:
    bool m_Switch = false;
};