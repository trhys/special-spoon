#include "DemoZombie.h"

#include <cstdlib>
#include <ctime>
#include <cmath>

std::srand(std::time(0));

void DemoZombie::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 10)
    {
        OnKill();
    }

    m_CurrentPosition = getPosition();

    const float angle = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0 * 3.14;
    const float distance = (static_cast<float>(std::rand()) / RAND_MAX) * 0.9 + 0.1;

    float dX = std::cos(angle) * distance * speed;
    float dY = std::sin(angle) * distance * speed;

    move({dX, dY});
}

void DemoZombie::CollisionDetected()
{
    setPosition(m_CurrentPosition);
}

void DemoZombie::OnKill()
{
    GetParent()->KillChild(this);
}

void MenuZombie::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 10)
    {
        OnKill();
    }

    move({10, 0});
}

void MenuZombie::OnKill()
{
    GetParent()->KillChild(this);
}

void ZombieSpawner::SpawnZombie()
{
    AddChild(new DemoZombie(GetParent()->LoadTexture("demozombie", "resources/DemoSprite.png")));
}

void ZombieSpawner::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 5)
    {
       SpawnZombie();
       timer = timer.Zero;
    }
}

void MenuZombieSpawner::SpawnMenuZombie()
{
    AddChild(new MenuZombie(GetParent()->LoadTexture("demozombie", "resources/DemoSprite.png")));
}

void MenuZombieSpawner::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 5)
    {
       SpawnMenuZombie();
       timer = timer.Zero;
    }
}