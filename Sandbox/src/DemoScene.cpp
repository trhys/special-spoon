#include "DemoScene.h"

DemoScene::DemoScene() : Spoon::Scene("demo_scene", {1080, 1080})
{
    AddChild(new Spoon::Entity(LoadTexture("demoscene", "resources/TD-Scene.png")));
    AddChild(new ZombieSpawner({500, 500}));
    AddChild(new ZombieSpawner({700, 700}));
}