#pragma once

#include "Spoon.h"
#include "DemoZombie.h"

class DemoScene : public Spoon::Scene
{
public:
    DemoScene();
    ~DemoScene() {};

    void SpawnZombie(ResourceManager& rsm);

    void SetScene() override;

private:
    void OnDraw(sf::RenderTarget& target, const sf::Transform& transform) const override

    std::map<std::string, std::filesystem::path> m_Resources = {
        {"zombiesprite", "resources/DemoSprite.png"},
        {"demoscene", "resources/TD-Scene.png"}
    }
};