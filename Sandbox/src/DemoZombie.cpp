#include "DemoZombie.h"

#include <random>
#include <chrono>


unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 engine(seed);
std::uniform_int_distribution<int> dist(1, 100);

void DemoZombie::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 10)
    {
        OnKill();
    }

    m_CurrentPosition = GetSpritePosition();

    const float angle = (dist(engine)) * 2.0 * 3.14;
    const float distance = (dist(engine)) * 0.9 + 0.1;

    float dX = std::cos(angle) * distance * (speed * tick.asSeconds());
    float dY = std::sin(angle) * distance * (speed * tick.asSeconds());

    move({dX, dY});
}

void DemoZombie::CollisionDetected()
{
    SetSpritePosition(m_CurrentPosition);
}

void MenuZombie::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 20)
    {
        OnKill();
    }

    move({100 * tick.asSeconds(), 0});
}

void ZombieSpawner::SpawnZombie()
{
    AddChild<DemoZombie>("demozombie");
}

void ZombieSpawner::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 10)
    {
       SpawnZombie();
       timer = timer.Zero;
    }
}

void MenuZombieSpawner::OnAdd()
{
    SpawnMenuZombie();
}

void MenuZombieSpawner::SpawnMenuZombie()
{
    AddChild<MenuZombie>("demozombie");
}

void MenuZombieSpawner::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 15)
    {
       SpawnMenuZombie();
       timer = timer.Zero;
    }
}