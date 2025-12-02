#include "DemoZombie.h"

#include <random>
#include <chrono>


unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 engine(seed);
std::uniform_int_distribution<int> dist(1, 100);

DemoZombie::DemoZombie(sf::Texture& asset, sf::Vector2f position, bool wander) : is_Wandering(wander)
{
    AddComponent<TransComp>(position);
    AddComponent<SpriteComp>(asset);
    GetComponent<SpriteComp>()->SetScale(0.25f, 0.25f);
    AddComponent<PhysComp>(GetComponent<SpriteComp>()->GetBoundingBox());
}

void DemoZombie::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 10)
    {
        OnKill();
    }

    if(is_Wandering)
    {
        const float angle = (dist(engine)) * 2.0 * 3.14;
        const float distance = (dist(engine)) * 0.9 + 0.1;

        float dX = std::cos(angle) * distance * (speed * tick.asSeconds());
        float dY = std::sin(angle) * distance * (speed * tick.asSeconds());

        GetComponent<TransComp>()->Move({dX, dY});
    }
    else
    {
        GetComponent<TransComp>()->Move({100.0f * tick.asSeconds(), 10.0f * tick.asSeconds()});
    }
}

void DemoZombie::CollisionDetected()
{
    GetComponent<TransComp>()->ResetPos();
}