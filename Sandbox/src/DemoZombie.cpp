#include "DemoZombie.h"

void DemoZombie::OnUpdate(sf::Time tick, Layer* context)
{
    move(speed * tick.asSeconds());
}

void ZombieSpawner::SpawnZombie(Layer* context)
{
    context->CreateNode<DemoZombie>(context->GetTexture("demozombie", "resources/DemoSprite.png"));
}

void ZombieSpawner::OnUpdate(sf::Time tick, Layer* context)
{
    timer += tick;
    if(timer > 5)
    {
        SpawnZombie(context);
        timer = 0;
    }
}