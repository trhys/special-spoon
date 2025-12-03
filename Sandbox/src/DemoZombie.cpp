#include "DemoZombie.h"

#include <random>
#include <chrono>


unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 engine(seed);
std::uniform_int_distribution<int> dist(1, 100);

DemoZombie::DemoZombie(sf::Texture& asset, sf::Vector2f position, bool wander) : is_Wandering(wander)
{
    AddComponent<Spoon::TransComp>(position);
    AddComponent<Spoon::SpriteComp>(asset);
    GetComponent<Spoon::SpriteComp>()->SetScale({ 0.25f, 0.25f });
    AddComponent<Spoon::PhysComp>(GetComponent<Spoon::SpriteComp>()->GetBoundingBox());
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

        float dX = std::cos(angle) * distance * (100.f * tick.asSeconds());
        float dY = std::sin(angle) * distance * (100.f * tick.asSeconds());

        GetComponent<Spoon::TransComp>()->Move({dX, dY});
    }
    else
    {
        GetComponent<Spoon::TransComp>()->Move({100.0f * tick.asSeconds(), 10.0f * tick.asSeconds()});
    }
}

void DemoZombie::CollisionDetected()
{
    GetComponent<Spoon::TransComp>()->ResetPos();
}