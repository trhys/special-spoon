#include "DemoZombie.h"

void DemoZombie::OnUpdate(sf::Time tick, Spoon::Layer* context)
{
    move(speed * tick.asSeconds());
}

void ZombieSpawner::SpawnZombie(Spoon::Layer* context)
{
    context->CreateNode<DemoZombie>(context->GetTexture("demozombie", "resources/DemoSprite.png"));
}

void ZombieSpawner::OnUpdate(sf::Time tick, Spoon::Layer* context)
{
    //timer = timer + tick;
    //if(timer.asSeconds() > 5)
    //{
    //    SpawnZombie(context);
    //    timer = timer.Zero;
    //}
}