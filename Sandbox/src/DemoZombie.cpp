#include "DemoZombie.h"

void DemoZombie::OnUpdate(sf::Time tick)
{
    move(speed * tick.asSeconds());
}

void ZombieSpawner::SpawnZombie()
{
    AddChild(new DemoZombie(GetParent()->GetAsset("demozombie")));
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