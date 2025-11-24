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

    m_CurrentPosition = getPosition();

    const float angle = (dist(engine)) * 2.0 * 3.14;
    const float distance = (dist(engine)) * 0.9 + 0.1;

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
    if(timer.asSeconds() > 20)
    {
        OnKill();
    }

    move({100 * tick.asSeconds(), 0});
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
    AddChild(new MenuZombie(GetParent()->LoadTexture("demozombie", "resources/DemoSprite.png")));
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